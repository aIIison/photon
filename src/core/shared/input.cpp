#include "core/interfaces/interfaces.h"
#include "sdk/photon.h"

#include <array>

static std::array< bool, 256 > cur_keys, old_keys;
static int                     cursor_x, cursor_y;

void c_input::poll_input( ) {
	std::copy( cur_keys.begin( ), cur_keys.end( ), old_keys.begin( ) );

	for ( std::size_t i = 0; i < 256; ++i ) {
		cur_keys[ i ] = interfaces::input_system->is_button_down( ( e_button_code ) i );
	}

	interfaces::input_system->get_cursor_position( &cursor_x, &cursor_y );
}

bool c_input::get_key_held( unsigned int code ) {
	return cur_keys[ code ];
}

bool c_input::get_key_release( unsigned int code ) {
	return !cur_keys[ code ] && old_keys[ code ];
}

bool c_input::get_key_press( unsigned int code ) {
	return cur_keys[ code ] && !old_keys[ code ];
}

vec2_t c_input::get_cursor_position( ) {
	return { cursor_x, cursor_y };
}

bool c_input::is_cursor_in_area( int x0, int y0, int x1, int y1 ) {
	return cursor_x >= x0 && cursor_y >= y0 && cursor_x <= x1 && cursor_y <= y1;
}
