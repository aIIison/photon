#include "sdk/hud.h"

#include "core/huds/huds.h"
#include "sdk/photon.h"

#include <cmath>
#include <cstddef>

void c_hud::reg( const char* name, photon_api::i_hud* hud ) {
	huds::huds.insert( { name, hud } );
}
void c_hud::unreg( const char* name ) {
	huds::huds.erase( name );
}

static photon_api::i_hud* cur_hud;

static inline void setup_context( int& x, int& y, int w, int h ) {
	cur_hud->bounds.x = std::fmax( cur_hud->bounds.x, x + w );
	cur_hud->bounds.y = std::fmax( cur_hud->bounds.y, y + h );

	const auto pos    = photon->render->to_screen( cur_hud->pos );
	const auto anchor = cur_hud->anchor * cur_hud->bounds;

	const auto screen_pos = pos - anchor;

	x += screen_pos.x;
	y += screen_pos.y;
}

void c_hud::draw_begin( photon_api::i_hud* hud ) {
	cur_hud = hud;
}
void c_hud::draw_end( ) {
	cur_hud = nullptr;
}
void c_hud::draw_filled_rect( int x, int y, int w, int h, const color_t& color ) {
	if ( !cur_hud )
		return;

	setup_context( x, y, w, h );

	photon->render->draw_filled_rect( x, y, w, h, color );
}
void c_hud::draw_outlined_rect( int x, int y, int w, int h, const color_t& color, int stroke_width ) {
	if ( !cur_hud )
		return;

	setup_context( x, y, w, h );

	photon->render->draw_outlined_rect( x, y, w, h, color, stroke_width );
}
void c_hud::draw_line( int x, int y, int w, int h, const color_t& color ) {
	if ( !cur_hud )
		return;

	setup_context( x, y, w, h );

	photon->render->draw_line( x, y, w, h, color );
}
void c_hud::draw_polygon( int n, vertex_t* vertices, const color_t& color ) {
	// FIXME: do some calculation here to figure out bounds
	// if ( !cur_hud )
	// 	return;

	// setup_context( x, y, w, h );

	// photon->render->draw_polygon( n, vertices, color );
}
void c_hud::draw_rounded_rect( int x, int y, int w, int h, const color_t& color, int rounding ) {
	if ( !cur_hud )
		return;

	setup_context( x, y, w, h );

	photon->render->draw_rounded_rect( x, y, w, h, color, rounding );
}
void c_hud::draw_circle( int x, int y, int radius, const color_t& color ) {
	if ( !cur_hud )
		return;

	setup_context( x, y, radius * 2, radius * 2 );  // XXX: maybe wrong ? too lazy to test rn

	photon->render->draw_circle( x, y, radius, color );
}
void c_hud::draw_text( int x, int y, h_font font, const color_t& color, bool center, const char* text ) {
	if ( !cur_hud )
		return;

	const auto text_size = photon->render->get_text_size( font, text );

	setup_context( x, y, text_size.x, text_size.y );

	photon->render->draw_text( x, y, font, color, center, text );
}
void c_hud::draw_texture( int x, int y, int w, int h, const char* texture, const color_t& color ) {
	if ( !cur_hud )
		return;

	setup_context( x, y, w, h );

	photon->render->draw_texture( x, y, w, h, texture, color );
}
void c_hud::draw_gradient( int x, int y, int w, int h, const color_t& color1, const color_t& color2, bool horizontal ) {
	if ( !cur_hud )
		return;

	setup_context( x, y, w, h );

	photon->render->draw_gradient( x, y, w, h, color1, color2, horizontal );
}
