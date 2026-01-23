#pragma once

#include "source_sdk.h"

namespace photon {
	class c_input {
	public:
		virtual void poll_input( );

		virtual bool get_key_held( unsigned int code );     // returns true while a key is being held.
		virtual bool get_key_release( unsigned int code );  // returns true on key release.
		virtual bool get_key_press( unsigned int code );    // returns true on key press.

		virtual vec2_t get_cursor_position( );                               // returns a datatype with two floats containing cursor x, y position.
		virtual bool   is_cursor_in_area( int x0, int y0, int x1, int y1 );  // returns true if the cursor is inside a rectangle based on its bounds.
	};
};  // namespace photon
