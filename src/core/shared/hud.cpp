#include "sdk/hud.h"

#include "core/huds/huds.h"
#include "sdk/photon.h"

#include <cmath>
#include <cstddef>

void c_hud::reg( const char* name, photon::i_hud* hud ) {
	huds::huds.insert( { name, hud } );
}
void c_hud::unreg( const char* name ) {
	huds::huds.erase( name );
}

static inline void setup_context( int& x, int& y, int w, int h ) {
	huds::cur_hud->bounds.x = std::fmax( huds::cur_hud->bounds.x, x + w );
	huds::cur_hud->bounds.y = std::fmax( huds::cur_hud->bounds.y, y + h );

	const auto pos    = photon::get( )->render->to_screen( huds::cur_hud->pos );
	const auto anchor = huds::cur_hud->anchor * huds::cur_hud->bounds;

	const auto screen_pos = pos - anchor;

	x += screen_pos.x;
	y += screen_pos.y;
}

void c_hud::draw_filled_rect( int x, int y, int w, int h, const color_t& color ) {
	if ( !huds::cur_hud )
		return;

	setup_context( x, y, w, h );

	photon::get( )->render->draw_filled_rect( x, y, w, h, color );
}
void c_hud::draw_outlined_rect( int x, int y, int w, int h, const color_t& color, int stroke_width ) {
	if ( !huds::cur_hud )
		return;

	setup_context( x, y, w, h );

	photon::get( )->render->draw_outlined_rect( x, y, w, h, color, stroke_width );
}
void c_hud::draw_line( int x, int y, int w, int h, const color_t& color ) {
	if ( !huds::cur_hud )
		return;

	setup_context( x, y, w, h );

	photon::get( )->render->draw_line( x, y, w, h, color );
}
void c_hud::draw_polygon( int n, vertex_t* vertices, const color_t& color ) {
	if ( !huds::cur_hud )
		return;

	// calculate bounds.
	vec2_t mins, maxs;
	mins = maxs = vertices[ 0 ].position;

	for ( int i = 1; i < n; ++i ) {
		const auto& pos = vertices[ i ].position;

		mins.x = std::fmin( mins.x, pos.x );
		mins.y = std::fmin( mins.y, pos.y );
		maxs.x = std::fmax( maxs.x, pos.x );
		maxs.y = std::fmax( maxs.y, pos.y );
	}

	int x = mins.x;
	int y = mins.y;

	// now setup.
	setup_context( x, y, maxs.x - mins.x, maxs.y - mins.y );

	// move each individual vertex.
	for ( int i{ }; i < n; ++i )
		vertices[ i ].position += { x, y };

	photon::get( )->render->draw_polygon( n, vertices, color );
}
void c_hud::draw_rounded_rect( int x, int y, int w, int h, const color_t& color, int rounding ) {
	if ( !huds::cur_hud )
		return;

	setup_context( x, y, w, h );

	photon::get( )->render->draw_rounded_rect( x, y, w, h, color, rounding );
}
void c_hud::draw_circle( int x, int y, int radius, const color_t& color ) {
	if ( !huds::cur_hud )
		return;

	setup_context( x, y, radius * 2, radius * 2 );

	x += radius;
	y += radius;

	photon::get( )->render->draw_circle( x, y, radius, color );
}
void c_hud::draw_text( int x, int y, h_font font, const color_t& color, bool center, const char* text ) {
	if ( !huds::cur_hud )
		return;

	const auto text_size = photon::get( )->render->get_text_size( font, text );

	setup_context( x, y, text_size.x, text_size.y );

	photon::get( )->render->draw_text( x, y, font, color, center, text );
}
void c_hud::draw_texture( int x, int y, int w, int h, const char* texture, const color_t& color ) {
	if ( !huds::cur_hud )
		return;

	setup_context( x, y, w, h );

	photon::get( )->render->draw_texture( x, y, w, h, texture, color );
}
void c_hud::draw_gradient( int x, int y, int w, int h, const color_t& color1, const color_t& color2, bool horizontal ) {
	if ( !huds::cur_hud )
		return;

	setup_context( x, y, w, h );

	photon::get( )->render->draw_gradient( x, y, w, h, color1, color2, horizontal );
}
