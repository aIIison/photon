#include "configs.h"

#include "core/huds/huds.h"
#include "core/mods/mods.h"
#include "util/util.h"

#include <fstream>
#include <nlohmann/json.hpp>

using namespace nlohmann::json_abi_v3_11_3::detail;

static nlohmann::json cfg;   // actual key:value json
static nlohmann::json ptrs;  // internal list of pointers to variables

static std::string cur_mod;

void configs::iterate_cfgs( ) {
	cfgs.clear( );

	std::filesystem::create_directory( "photon/cfgs" );
	for ( const auto& entry : std::filesystem::directory_iterator( "photon/cfgs" ) ) {
		auto path = entry.path( );
		if ( path.extension( ) != ".json" )
			continue;

		cfgs.push_back( path.stem( ).string( ) );
	}
}

bool configs::initialize( ) {
	iterate_cfgs( );

	return true;
}

void configs::uninitialize( ) {
}

void configs::save( const char* cfg_name ) {
	// save loaded modules.
	for ( auto& [ name, mod ] : mods::mod_list ) {
		cfg[ "modules" ][ name ] = mod.is_loaded;
	}

	// save hud layout.
	for ( auto& [ name, hud ] : huds::huds ) {
		auto& cur_hud = cfg[ "huds" ][ name ];

		cur_hud[ "pos" ][ "x" ]    = hud->pos.x;
		cur_hud[ "pos" ][ "y" ]    = hud->pos.y;
		cur_hud[ "anchor" ][ "x" ] = hud->anchor.x;
		cur_hud[ "anchor" ][ "y" ] = hud->anchor.y;

		cur_hud[ "scale" ] = hud->scale;
	}

	// save module settings.
	for ( auto& [ module, obj ] : cfg.items( ) ) {
		if ( module == "modules" )
			continue;

		for ( auto& [ key, value ] : obj.items( ) ) {
			if ( !ptrs[ module ].contains( key ) )
				continue;

			address_t p = ptrs[ module ][ key ].get< uintptr_t >( );

			switch ( value.type( ) ) {
			case value_t::array: /* array = color_t */ {
				color_t col = p.get< color_t >( );
				value       = { col.r, col.g, col.b, col.a };
				continue;
			}
			case value_t::string:
				value = p.get< const char* >( );
				continue;
			case value_t::boolean:
				value = p.get< bool >( );
				continue;
			case value_t::number_integer:
				value = p.get< int32_t >( );
				continue;
			case value_t::number_unsigned:
				value = p.get< uint32_t >( );
				continue;
			case value_t::number_float:
				value = p.get< float >( );
				continue;
			}
		}
	}

	std::ofstream o( util::ssprintf( "photon/cfgs/%s.json", cfg_name ) );
	o << std::setw( 4 ) << cfg << std::endl;
}

void configs::load( const char* cfg_name ) {
	std::ifstream i( util::ssprintf( "photon/cfgs/%s.json", cfg_name ) );
	i >> cfg;

	// load saved modules.
	for ( auto& [ name, mod ] : mods::mod_list ) {
		bool enabled = cfg[ "modules" ][ name ].get< bool >( );

		if ( enabled )
			mods::enable( &mod );
		else
			mods::disable( &mod );
	}

	// load hud layout.
	for ( auto& [ name, hud ] : huds::huds ) {
		auto& cur_hud = cfg[ "huds" ][ name ];

		hud->pos.x    = cur_hud[ "pos" ][ "x" ].get< float >( );
		hud->pos.y    = cur_hud[ "pos" ][ "y" ].get< float >( );
		hud->anchor.x = cur_hud[ "anchor" ][ "x" ].get< float >( );
		hud->anchor.y = cur_hud[ "anchor" ][ "y" ].get< float >( );

		hud->scale = cur_hud[ "scale" ].get< float >( );
	}

	// load module settings.
	for ( auto& [ module, obj ] : cfg.items( ) ) {
		if ( !ptrs.contains( module ) || module == "modules" )
			continue;

		for ( auto& [ key, value ] : obj.items( ) ) {
			if ( !ptrs[ module ].contains( key ) )
				continue;

			address_t p = ptrs[ module ][ key ].get< uintptr_t >( );

			switch ( value.type( ) ) {
			case value_t::array: /* array = color_t */
				p.set( address_t{ value.get< std::vector< std::uint8_t > >( ).data( ) }.get< uint32_t >( ) );
				continue;
			case value_t::string:
				p.set( strdup( value.get< std::string >( ).c_str( ) ) );
				continue;
			case value_t::boolean:
				p.set( value.get< bool >( ) );
				continue;
			case value_t::number_integer:
				p.set( value.get< int32_t >( ) );
				continue;
			case value_t::number_unsigned:
				p.set( value.get< uint32_t >( ) );
				continue;
			case value_t::number_float:
				p.set( value.get< float >( ) );
				continue;
			}
		}
	}
}

void configs::create( const char* cfg_name ) {
	save( cfg_name );
	iterate_cfgs( );
}

void configs::remove( const char* cfg_name ) {
	std::remove( util::ssprintf( "photon/cfgs/%s.json", cfg_name ).c_str( ) );
	iterate_cfgs( );
}

void configs::begin( const char* module_name ) {
	cur_mod = module_name;
}
void configs::end( ) {
	cur_mod.clear( );
}

#define SERIALIZE( var_name, var )                  \
	{                                                  \
		if ( cur_mod.empty( ) )                           \
			return;                                          \
		cfg[ cur_mod ][ var_name ]  = var;                \
		ptrs[ cur_mod ][ var_name ] = ( uintptr_t ) &var; \
	}

void configs::serialize( const char* var_name, const char*& var ) SERIALIZE( var_name, var );
void configs::serialize( const char* var_name, bool& var ) SERIALIZE( var_name, var );
void configs::serialize( const char* var_name, int32_t& var ) SERIALIZE( var_name, var );
void configs::serialize( const char* var_name, uint32_t& var ) SERIALIZE( var_name, var );
void configs::serialize( const char* var_name, float& var ) SERIALIZE( var_name, var );
void configs::serialize( const char* var_name, color_t& var ) {
	if ( cur_mod.empty( ) )
		return;
	cfg[ cur_mod ][ var_name ]  = { var.r, var.g, var.b, var.a };
	ptrs[ cur_mod ][ var_name ] = ( uintptr_t ) &var;
}
void configs::deserialize( const char* module_name ) {
	cfg.erase( module_name );
	ptrs.erase( module_name );
}
