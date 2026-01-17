#include "photon.h"

#include "core/configs/configs.h"
#include "core/convars/convars.h"
#include "core/debugger/debugger.h"
#include "core/interfaces/interfaces.h"
#include "core/menu/gui.h"
#include "core/mods/mods.h"
#include "core/signals/signals.h"
#include "util/util.h"

#include <cstring>

c_photon plugin;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( c_photon, i_server_plugin_callbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, plugin );

photon::factory_t photon::get;
DLL_EXPORT photon::c_shared* photon_factory( ) {
	static photon::c_shared f;
	return &f;
}

// prevent from loading the plugin twice (why doesnt source do this ???)
static void plugin_load( const c_command& args ) {
	if ( args.arg_c( ) >= 2 && strstr( args[ 1 ], "photon" ) )
		photon::get( )->common->log_warn( "photon is already loaded.\n" );
	else
		plugin_load( args );
}

// we need to unhook cengine::frame before the plugin gets unloaded
static void plugin_unload( const c_command& args ) {
	if ( args.arg_c( ) >= 2 && plugin.get_info( ) && ( !strcmp( args[ 1 ], "photon" ) || std::atoi( args[ 1 ] ) == plugin.info->index ) )
		plugin.unload( );
	else
		plugin_unload( args );
}

bool c_photon::load( create_interface_fn interface_factory, create_interface_fn game_server_factory ) {
	info = new plugin_info_t( );

	photon::get            = &photon_factory;
	photon::get( )->common = new c_common( );
	photon::get( )->con    = new c_con( );
	photon::get( )->signal = new c_signal( );
	photon::get( )->hud    = new c_hud( );
	photon::get( )->render = new c_render( );
	photon::get( )->input  = new c_input( );
	photon::get( )->menu   = new c_menu( );
	photon::get( )->config = new c_config( );

#ifdef _DEBUG
	debugger::initialize( );
#endif

	if ( !interfaces::initialize( ) ) {
		photon::get( )->common->log_warn( "failed to initialize one or more interfaces.\n" );
		return false;
	}

	if ( util::get_module_handle( MODULE( "sar" ) ) ) {
		photon::get( )->common->log_warn( "this plugin is incompatible with SAR, please restart your game without loading SAR.\n" );
		return false;
	}

	if ( !signals::initialize( ) ) {
		photon::get( )->common->log_warn( "failed to initialize one or more signals.\n" );
		return false;
	}

	if ( !convars::initialize( ) ) {
		photon::get( )->common->log_warn( "failed to initialize one or more convars.\n" );
		return false;
	}

	if ( !configs::initialize( ) ) {
		photon::get( )->common->log_warn( "failed to initialize configs.\n" );
		return false;
	}

	if ( !gui::initialize( ) ) {
		photon::get( )->common->log_warn( "failed to initialize gui.\n" );
		return false;
	}

	photon::get( )->con->hook_cmd( "plugin_load", &plugin_load );
	photon::get( )->con->hook_cmd( "plugin_unload", &plugin_unload );

	// only works when done early enough
	interfaces::command_line->append_parm( "-background", "5" );

	photon::get( )->common->log( { 0, 255, 0, 255 }, "photon loaded.\n" );

	if ( !mods::loadall( ) )
		photon::get( )->common->log_warn( "failed to load one or more mods.\n" );

	return true;
}

bool c_photon::get_info( ) {
	auto size = interfaces::server_plugin_helpers.at< int >( 0x14 );
	if ( size > 0 ) {
		auto plugins = interfaces::server_plugin_helpers.at< c_plugin** >( 0x4 );
		for ( auto i = 0; i < size; ++i ) {
			auto ptr = plugins[ i ];
			if ( !std::strcmp( ptr->name, PLUGIN_SIGNATURE ) ) {
				info->ptr   = ptr;
				info->index = i;
				return true;
			}
		}
	}
	return false;
}

void c_photon::unload( ) {
	if ( unloading )
		return;
	unloading = true;

	mods::unloadall( );

	// this doesnt really do anything lol
	interfaces::command_line->remove_parm( "-background" );

	photon::get( )->con->unhook_cmd( "plugin_unload" );
	photon::get( )->con->unhook_cmd( "plugin_load" );

	gui::uninitialize( );
	configs::uninitialize( );
	convars::uninitialize( );
	signals::uninitialize( );

	if ( plugin.get_info( ) ) {
		auto unload_cmd = std::string( "plugin_unload " ) + std::to_string( plugin.info->index );
		auto target     = interfaces::engine_client->get_active_split_screen_slot( );
		interfaces::engine_client->cbuf_add_text( target, unload_cmd.c_str( ), SAFE_UNLOAD_DELAY );
	}

	photon::get( )->common->log( "goodbye.\n" );

	interfaces::uninitialize( );

	debugger::uninitialize( );

	SAFE_DELETE( photon::get( )->config );
	SAFE_DELETE( photon::get( )->menu );
	SAFE_DELETE( photon::get( )->input );
	SAFE_DELETE( photon::get( )->render );
	SAFE_DELETE( photon::get( )->hud );
	SAFE_DELETE( photon::get( )->common );
	SAFE_DELETE( photon::get( )->signal );
	SAFE_DELETE( photon::get( )->con );
	SAFE_DELETE( photon::get( )->common );
	SAFE_DELETE( info );
}
