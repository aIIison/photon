#pragma once

#include "util/util.h"

class i_engine_client {
public:
	__forceinline void cbuf_add_text( int target, const char* text, int tick_delay ) {
		static auto client_cmd = util::get_virtual< 7 >( this );
		client_cmd.rel32< void( __cdecl* )( int, const char*, int ) >( OS( 0x2e, 0x23 ) )( target, text, tick_delay );
	}
	__forceinline void client_cmd( const char* cmd_string ) {
		return util::call_virtual< 7, void >( this, cmd_string );
	}
	__forceinline bool con_is_visible( ) {
		return util::call_virtual< 11, bool >( this );
	}
	__forceinline int get_local_player( ) {
		return util::call_virtual< 12, int >( this );
	}
	__forceinline const char* get_level_name_short( ) {
		return util::call_virtual< 53, const char* >( this );
	}
	__forceinline void execute_client_cmd( const char* cmd_string ) {
		return util::call_virtual< 104, void >( this, cmd_string );
	}
	__forceinline int get_active_split_screen_slot( ) {
		return util::call_virtual< 127, int >( this );
	}
	__forceinline void* get_input_context( int id ) {
		return util::call_virtual< 190, void* >( this, id );
	}
};
