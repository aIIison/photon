#include "core/interfaces/interfaces.h"
#include "sdk/photon.h"

#include <lodepng/lodepng.h>
#include <unordered_map>

static std::unordered_map< std::string, int > texture_ids;

void c_render::draw_filled_rect( int x, int y, int w, int h, const color_t& color ) {
	interfaces::surface->draw_set_color( color.r, color.g, color.b, color.a );
	interfaces::surface->draw_filled_rect( x, y, x + w, y + h );
}

void c_render::draw_outlined_rect( int x, int y, int w, int h, const color_t& color, int stroke_width ) {
	interfaces::surface->draw_set_color( color.r, color.g, color.b, color.a );
	for ( int i = 0; i < stroke_width; ++i ) {
		interfaces::surface->draw_outlined_rect( x - i, y - i, x + w + i, y + h + i );
	}
}

void c_render::draw_line( int x, int y, int w, int h, const color_t& color ) {
	interfaces::surface->draw_set_color( color.r, color.g, color.b, color.a );
	interfaces::surface->draw_line( x, y, x + w, y + h );
}

void c_render::draw_polygon( int n, vertex_t* vertices, const color_t& color ) {
	// create plain white texture
	static int     plain_tex = interfaces::surface->create_new_texture_id( true );
	static uint8_t rgba[ 4 ] = { 255, 255, 255, 255 };
	interfaces::surface->draw_set_texture_rgba( plain_tex, rgba, 1, 1 );

	interfaces::surface->draw_set_texture( plain_tex );
	interfaces::surface->draw_set_color( color.r, color.g, color.b, color.a );

	interfaces::surface->draw_textured_polygon( n, vertices );
}

void c_render::draw_rounded_rect( int x, int y, int w, int h, const color_t& color, int rounding ) {
	if ( rounding < 2 )
		return;

	const int n_verts = rounding * 4;

	vertex_t* verts = new vertex_t[ 4 * n_verts ];
	for ( int i = 0; i < 4; ++i ) {
		int _x = x + ( i < 2 ? w - rounding : rounding );
		int _y = y + ( i % 3 ? h - rounding : rounding );

		float ang = 90.f * i;

		for ( int j = 0; j < n_verts; j++ ) {
			float rad = math::deg_to_rad( ang + ( j / ( float ) ( n_verts - 1 ) ) * 90.f );

			verts[ i * n_verts + j ] = {
				{ _x + rounding * sin( rad ),
				  _y - rounding * cos( rad ) }
			};
		}
	}

	draw_polygon( 4 * rounding * 4, verts, color );

	delete[] verts;
}

void c_render::draw_circle( int x, int y, int radius, const color_t& color ) {
	vertex_t verts[ 360 ];
	for ( int i = 0; i < 360; ++i ) {
		auto& pos = verts[ i ].position;

		float rad = math::deg_to_rad( i );

		pos.x = x + cos( rad ) * radius;
		pos.y = y + sin( rad ) * radius;
	}

	draw_polygon( 360, verts, color );
}

void c_render::draw_text( int x, int y, h_font font, const color_t& color, bool center, const char* text ) {
	int text_x = x;
	int text_y = y;

	if ( center ) {
		auto text_size = get_text_size( font, text );
		text_x         = x - text_size.x / 2;
	}

	interfaces::surface->draw_colored_text( font, text_x, text_y, color.r, color.g, color.b, color.a, text );
}

void c_render::draw_texture( int x, int y, int w, int h, const char* texture, const color_t& color ) {
	// check if texture already exists
	int id = interfaces::surface->draw_get_texture_id( texture );
	if ( id < 1 ) {
		// check if its a user-loaded texture
		if ( texture_ids.contains( texture ) )
			id = texture_ids[ texture ];
		else {
			// doesnt exist at all, fallback to loading it from game
			id = interfaces::surface->create_new_texture_id( true );
			interfaces::surface->draw_set_texture_file( id, texture, false, true );
		}
	}

	interfaces::surface->draw_set_texture( id );
	interfaces::surface->draw_set_color( color.r, color.g, color.b, color.a );
	interfaces::surface->draw_textured_rect( x, y, x + w, y + h );
}

void c_render::draw_gradient( int x, int y, int w, int h, const color_t& color1, const color_t& color2, bool horizontal ) {
	interfaces::surface->draw_set_color( color1.r, color1.g, color1.b, color1.a );
	interfaces::surface->draw_filled_rect( x, y, x + w, y + h );

	interfaces::surface->draw_set_color( color2.r, color2.g, color2.b, color2.a );
	interfaces::surface->draw_filled_rect_fade( x, y, x + w, y + h, 0, 255, horizontal );
}

void c_render::load_texture_raw( const char* name, const uint8_t* rgba, int w, int h ) {
	int id = interfaces::surface->create_new_texture_id( true );
	interfaces::surface->draw_set_texture_rgba( id, rgba, w, h );

	texture_ids.insert( std::make_pair( name, id ) );
}

void c_render::load_texture_png( const char* name, const uint8_t* png, int w, int h, size_t size ) {
	std::vector< uint8_t > rgba;

	auto _w = std::uint32_t( w );
	auto _h = std::uint32_t( h );

	lodepng::decode( rgba, _w, _h, png, size );

	load_texture_raw( name, rgba.data( ), w, h );
}

bool c_render::create_font( h_font& font, const char* font_name, int size, bool bold, int flags ) {
	font = interfaces::surface->create_font( );
	return interfaces::surface->set_font_glyph_set( font, font_name, size, bold ? 700 : 400, 0, 1, flags );
}

void c_render::destruct_font( h_font font ) {
	/*
	 * seems like the game doesnt have a way of destructing a specific font,
	 * so we have to do it ourselves
	 */

	for ( const auto& font_range : interfaces::font_manager->font_amalgams[ font ].fonts ) {
		const int idx = interfaces::font_manager->win32_fonts.find( font_range.win32_font );

		interfaces::mem_alloc->free( interfaces::font_manager->win32_fonts[ idx ] );
		interfaces::font_manager->win32_fonts.remove( idx );
	}

	interfaces::font_manager->font_amalgams.remove( font );
}

vec2_t c_render::get_text_size( h_font font, const char* text ) {
	int text_width, text_height;

	std::string stxt = text;
	interfaces::surface->get_text_size( font, std::wstring( stxt.begin( ), stxt.end( ) ).c_str( ), text_width, text_height );

	return { text_width, text_height };
}

vec2_t c_render::get_screen_size( ) {
	int w, h;

	interfaces::surface->get_screen_size( w, h );

	return { w, h };
}

h_font c_render::get_font( unsigned long id ) {
	auto default_font = interfaces::scheme->get_font( "DefaultFixedOutline" );

	return default_font + id;
}

vec2_t c_render::normalize( const vec2_t& vec ) {
	const auto screen_size = get_screen_size( );

	return vec / screen_size;
}

vec2_t c_render::to_screen( const vec2_t& vec ) {
	const auto screen_size = get_screen_size( );

	return vec * screen_size;
}
