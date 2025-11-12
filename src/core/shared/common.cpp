#include "sdk/common.h"

#include "core/interfaces/interfaces.h"
#include "core/mods/mods.h"
#include "util/util.h"

#include <stdarg.h>

void c_common::log( const char* fmt, ... ) {
	/* this sucks */
	va_list ap1, ap2;
	va_start( ap1, fmt );
	va_copy( ap2, ap1 );
	size_t sz = vsnprintf( NULL, 0, fmt, ap1 ) + 1;
	va_end( ap1 );
	char* buf = ( char* ) malloc( sz );
	vsnprintf( buf, sz, fmt, ap2 );
	va_end( ap2 );
	std::string str( buf );
	free( buf );
	interfaces::console->msg( str.c_str( ) );
	util::console::log( "[+] %s", str.c_str( ) );
}
void c_common::log( const color_t& clr, const char* fmt, ... ) {
	/* this sucks */
	va_list ap1, ap2;
	va_start( ap1, fmt );
	va_copy( ap2, ap1 );
	size_t sz = vsnprintf( NULL, 0, fmt, ap1 ) + 1;
	va_end( ap1 );
	char* buf = ( char* ) malloc( sz );
	vsnprintf( buf, sz, fmt, ap2 );
	va_end( ap2 );
	std::string str( buf );
	free( buf );
	interfaces::console->color_msg( clr, str.c_str( ) );
	util::console::log( PRINT_GREEN "[+] %s", str.c_str( ) );
}
void c_common::log_warn( const char* fmt, ... ) {
	/* this sucks */
	va_list ap1, ap2;
	va_start( ap1, fmt );
	va_copy( ap2, ap1 );
	size_t sz = vsnprintf( NULL, 0, fmt, ap1 ) + 1;
	va_end( ap1 );
	char* buf = ( char* ) malloc( sz );
	vsnprintf( buf, sz, fmt, ap2 );
	va_end( ap2 );
	std::string str( buf );
	free( buf );
	interfaces::console->warning( str.c_str( ) );
	util::console::log( PRINT_RED "[!] %s", str.c_str( ) );
}
void c_common::post_event( void* sender, const char* msg ) {
	mods::post_event( sender, msg );
}

void* c_common::get_interface( const char* module_name, const char* interface_name ) {
	return util::get_interface( module_name, interface_name );
}
void* c_common::pattern_scan( const char* module_name, const char* signature ) {
	return util::pattern_scan( module_name, signature );
}
void* c_common::get_module_handle( const char* module_name ) {
	return util::get_module_handle( module_name );
}

int c_common::get_slot( ) {
	return interfaces::engine_client->get_local_player( );
}
bool c_common::is_coop( ) {
	// FIXME: unfinished
	if ( !strlen( get_current_map( ) ) )
		return false;

	if ( *interfaces::game_rules )
		return ( *interfaces::game_rules )->is_multiplayer( );

	static auto sv_portal_players = photon->con->find_con_var( "sv_portal_players" );
	return sv_portal_players->get_int( ) == 2;
}
bool c_common::is_orange( ) {
	// FIXME: unfinished
	static bool is_orange;

	is_orange = is_coop( ) && !get_host_state( )->active_game;

	return is_orange;
}
bool c_common::is_splitscreen( ) {
	// FIXME: unfinished
	if ( !is_coop( ) )
		return false;

	for ( int i = 0; i < 2; ++i ) {
		void* player = get_client_entity( i + 1 );
		if ( !player )
			continue;
	}

	return false;
}
const char* c_common::get_current_map( ) {
	return interfaces::engine_client->get_level_name_short( );
}
c_host_state* c_common::get_host_state( ) {
	return interfaces::host_state;
}
void* c_common::get_client_entity( int entnum ) {
	return interfaces::entity_list->get_client_entity( entnum );
}
