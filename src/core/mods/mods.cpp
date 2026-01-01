#include "mods.h"

#include "core/interfaces/interfaces.h"
#include "core/photon.h"
#include "util/util.h"

#include <filesystem>
#include <iostream>

bool mods::load( const char* name ) {
	if ( mod_list.count( name ) )
		return false;

#ifdef _WIN32
	void* lib = LoadLibraryA( util::ssprintf( "photon/%s.dll", name ).c_str( ) );
#else
	void* lib = dlopen( util::ssprintf( "photon/%s.so", name ).c_str( ), RTLD_NOW );
#endif

	if ( lib ) {
		using create_mod_fn = photon_api::i_photon_mod* ( * ) ( );
		const auto fn       = util::get_sym_addr< create_mod_fn >( lib, "create_mod" );

		if ( fn ) {
			auto mod = fn( );

			if ( !mod )
				return false;

			mod_info_t info;
			info.handle    = lib;
			info.ptr       = mod;
			info.is_loaded = false;

			mod_list.insert( std::make_pair( name, info ) );

			util::console::log( "[+] loaded library `%s`.\n", name );

			return true;
		}
	}

#ifdef _WIN32
	photon->common->log_warn( "failed to load library `%s` (%lu).\n", name, GetLastError( ) );
#else
	photon->common->log_warn( "failed to load library `%s` (%s).\n", name, dlerror( ) );
#endif

	return false;
}

void mods::unload( const char* name ) {
	if ( !mod_list.count( name ) )
		return;

	auto& mod = mod_list[ name ];

	if ( mod.is_loaded )
		mod.ptr->unload( );

#ifdef _WIN32
	FreeLibrary( ( HMODULE ) mod.handle );
#else
	dlclose( mod.handle );
#endif

	mod_list.erase( name );
}

bool mods::loadall( ) {
	bool had_fail = false;
	for ( const auto& entry : std::filesystem::directory_iterator( "photon" ) ) {
		auto path = entry.path( );
		if ( path.extension( ) != OS( ".dll", ".so" ) )
			continue;

		bool result = load( path.stem( ).string( ).c_str( ) );

		if ( !result ) {
			had_fail = true;
		}
	}

	return !had_fail;
}

void mods::unloadall( ) {
	for ( const auto& mod : mod_list ) {
		if ( mod.second.is_loaded )
			mod.second.ptr->unload( );

#ifdef _WIN32
		FreeLibrary( ( HMODULE ) mod.second.handle );
#else
		dlclose( mod.second.handle );
#endif
	}

	mod_list.clear( );
}

bool mods::enable( mod_info_t* mod ) {
	bool result = false;
	if ( !mod->is_loaded ) {
		result         = mod->ptr->load( photon );
		mod->is_loaded = true;
	} else
		return false;

	if ( result )
		util::console::log( "[+] successfully enabled mod `%s`.\n", mod->ptr->get_info( ).name );
	else
		util::console::log_error( "[!] failed to enable mod `%s`.\n", mod->ptr->get_info( ).name );

	return result;
}

void mods::disable( mod_info_t* mod ) {
	if ( !mod->is_loaded )
		return;

	mod->is_loaded = false;
	mod->ptr->unload( );

	util::console::log( "[-] disabled mod `%s`.\n", mod->ptr->get_info( ).name );
}

void mods::print( ) {
	photon->common->log( "loaded photon mods (%d):\n", mod_list.size( ) );
	for ( const auto& mod : mod_list ) {
		const char* status = mod.second.is_loaded ? "enabled" : "disabled";
		const auto  info   = mod.second.ptr->get_info( );

		photon->common->log( "%s: %s@%s (%s)\n", mod.first.c_str( ), info.name, info.version, status );
	}
}

void mods::post_event( void* sender, const char* msg ) {
	auto msg_s = std::string( msg );

	if ( sender != &plugin ) {
		const auto mod = reinterpret_cast< photon_api::i_photon_mod* >( sender );
		msg_s          = std::string( mod->get_info( ).name ) + std::string( ":" ) + msg_s;
	}

	for ( const auto& mod : mod_list ) {
		if ( mod.second.is_loaded )
			mod.second.ptr->on_event( msg_s.c_str( ) );
	}
}
