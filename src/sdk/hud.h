#pragma once

#include "source_sdk.h"

#include <vector>

namespace photon {
	class c_hud_context {
	public:
		virtual void draw_filled_rect( int x, int y, int w, int h, const color_t& color );
		virtual void draw_outlined_rect( int x, int y, int w, int h, const color_t& color, int stroke_width = 1 );
		virtual void draw_line( int x, int y, int w, int h, const color_t& color );
		virtual void draw_polygon( int n, vertex_t* vertices, const color_t& color );
		virtual void draw_rounded_rect( int x, int y, int w, int h, const color_t& color, int rounding );
		virtual void draw_circle( int x, int y, int radius, const color_t& color );
		virtual void draw_text( int x, int y, h_font font, const color_t& color, bool center, const char* text );
		virtual void draw_texture( int x, int y, int w, int h, const char* texture, const color_t& color = { } );
		virtual void draw_gradient( int x, int y, int w, int h, const color_t& color1, const color_t& color2, bool horizontal );
	};

	class i_hud {
	public:
		vec2_t pos;     // values between (0, 1).
		vec2_t anchor;  // values between (0, 1).
		float  scale;   // scale value to multiply by. (currently useless)
		vec2_t bounds;  // screen boundaries.

		virtual void draw( c_hud_context* ctx ) = 0;
		virtual bool is_splitscreen( )          = 0;
	};

	class c_hud {
	public:
		virtual void reg( const char* name, photon::i_hud* hud );
		virtual void unreg( const char* name );
	};
}  // namespace photon
