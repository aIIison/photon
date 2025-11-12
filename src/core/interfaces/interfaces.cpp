#include "interfaces.h"

#include "sdk/photon.h"
#include "util/util.h"

#define LOG_PTR( ptr )                                                                                    \
	if ( ptr )                                                                                               \
		util::console::log( "[+] pointer " PRINT_YELLOW "%s" PRINT_CYAN " [%p]" PRINT_RESET ".\n", #ptr, ptr ); \
	else                                                                                                     \
		util::console::log( PRINT_RED "[!] %s is nullptr.\n", #ptr );

bool interfaces::initialize( ) {
	console = new c_console( util::get_module_handle( OS( "tier0.dll", "libtier0.so" ) ) );

	cvar                  = util::get_interface( OS( "vstdlib.dll", "libvstdlib.so" ), "VEngineCvar007" ).as< i_cvar* >( );
	engine_api            = util::get_interface( MODULE( "engine" ), "VENGINE_LAUNCHER_API_VERSION004" );
	engine_client         = util::get_interface( MODULE( "engine" ), "VEngineClient015" ).as< i_engine_client* >( );
	server_plugin_helpers = util::get_interface( MODULE( "engine" ), "ISERVERPLUGINHELPERS001" );
	surface               = util::get_interface( MODULE( "vguimatsurface" ), "VGUI_Surface031" ).as< i_surface* >( );
	input_system          = util::get_interface( MODULE( "inputsystem" ), "InputSystemVersion001" ).as< i_input_system* >( );
	scheme_manager        = util::get_interface( MODULE( "vgui2" ), "VGUI_Scheme010" ).as< i_scheme_manager* >( );
	input_stack_system    = util::get_interface( MODULE( "inputsystem" ), "InputStackSystemVersion001" ).as< i_input_stack_system* >( );
	entity_list           = util::get_interface( MODULE( "client" ), "VClientEntityList003" ).as< i_client_entity_list* >( );

	const auto tier0 = util::get_module_handle( OS( "tier0.dll", "libtier0.so" ) );
	mem_alloc        = *util::get_sym_addr< i_mem_alloc** >( tier0, "g_pMemAlloc" );
	command_line     = util::get_sym_addr< i_command_line* ( * ) ( ) >( tier0, "CommandLine" )( );

	engine       = util::get_virtual< 12 >( interfaces::engine_api ).at( OS( 0x2, 0x4 ) ).get( );
	client_state = util::get_virtual< 7 >( interfaces::engine_client ).rel32< void* ( * ) ( ) >( OS( 0x4, 0x9 ) )( );
	font_manager = util::get_virtual< 132 >( interfaces::surface ).rel32< c_font_manager* ( * ) ( ) >( OS( 0x8, 0x9 ) )( );
	scheme       = interfaces::scheme_manager->get_i_scheme( 1 );
	game_rules   = util::pattern_scan( MODULE( "client" ), "E8 ? ? ? ? C7 06 ? ? ? ? 89 7E 0C" ).rel32( 0x1 ).at< c_game_rules** >( 0x15 );  // TODO: linux
	host_state   = util::get_virtual< OS( 15, 36 ) >( interfaces::client_state ).rel32( OS( 0x2ac, 0x4fb ) ).at< c_host_state* >( OS( 0x1, 0x1c ) );

	LOG_PTR( mem_alloc );
	LOG_PTR( command_line );
	LOG_PTR( engine );
	LOG_PTR( client_state );
	LOG_PTR( font_manager );
	LOG_PTR( scheme );
	LOG_PTR( game_rules );
	LOG_PTR( host_state );

	return true;
}

void interfaces::uninitialize( ) {
	SAFE_DELETE( console );
}
