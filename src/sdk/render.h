#pragma once

#include "source_sdk.h"

#include <string>

namespace photon {
	class c_render {
	public:
		virtual void   draw_filled_rect( int x, int y, int w, int h, const color_t& color );
		virtual void   draw_outlined_rect( int x, int y, int w, int h, const color_t& color, int stroke_width = 1 );
		virtual void   draw_line( int x, int y, int w, int h, const color_t& color );
		virtual void   draw_polygon( int n, vertex_t* vertices, const color_t& color );
		virtual void   draw_rounded_rect( int x, int y, int w, int h, const color_t& color, int rounding );
		virtual void   draw_circle( int x, int y, int radius, const color_t& color );
		virtual void   draw_text( int x, int y, h_font font, const color_t& color, int align, const char* text );  // align: 0 = left, 1 = center, 2 = right.
		virtual void   draw_texture( int x, int y, int w, int h, const char* texture, const color_t& color = { } );
		virtual void   draw_gradient( int x, int y, int w, int h, const color_t& color1, const color_t& color2, bool horizontal );
		virtual void   load_texture_raw( const char* name, const uint8_t* rgba, int w, int h );
		virtual void   load_texture_png( const char* name, const uint8_t* png, int w, int h, size_t size );
		virtual bool   create_font( h_font& font, const char* font_name, int size, bool bold, int flags );
		virtual void   destruct_font( h_font font );
		virtual vec2_t get_text_size( h_font font, const char* text );
		virtual vec2_t get_screen_size( );
		virtual h_font get_font( unsigned long id );
		virtual vec2_t normalize( const vec2_t& vec );  // translate screen x, y pixel position to a normal value between (0, 1).
		virtual vec2_t to_screen( const vec2_t& vec );  // translate x, y position between (0, 1) to a position on the screen.
	};
};  // namespace photon
