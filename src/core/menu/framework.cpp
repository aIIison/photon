#include "framework.h"

#include "core/configs/configs.h"
#include "core/interfaces/interfaces.h"
#include "core/mods/mods.h"
#include "util/util.h"

#include <algorithm>

static inline vec2_t align_right( const vec2_t& pos, const vec2_t& size ) {
	auto pos_x = gui::framework::cur_menu.pos.x + gui::framework::cur_menu.size.x - size.x - 20;

	return { pos_x, pos.y };
}

void gui::framework::set_theme( bool dark ) {
	if ( !dark ) {
		colors::bg       = { 255, 255, 255, 255 };
		colors::fg       = { 200, 200, 200, 255 };
		colors::text     = { 0, 0, 0, 200 };
		colors::disabled = { 0, 0, 0, 64 };
	} else {
		colors::bg       = { 20, 20, 20, 255 };
		colors::fg       = { 60, 60, 60, 255 };
		colors::text     = { 255, 255, 255, 48 };
		colors::disabled = { 255, 255, 255, 4 };
	}
}

void gui::framework::begin( const vec2_t& pos, const vec2_t& size ) {
	cur_menu = { };

	cur_menu.pos  = pos;
	cur_menu.size = size;

	cur_menu.cursor = { 8, 8 };

	// block input to other controls when a dropdown or colorpicker is open
	cur_menu.block_input = !cur_dropdown.id.empty( ) || !cur_colorpicker.id.empty( );

	photon->render->draw_rounded_rect( cur_menu.pos.x, cur_menu.pos.y, cur_menu.size.x, cur_menu.size.y, colors::bg, 8 );

	interfaces::surface->enable_clipping = true;
	interfaces::surface->set_clip_rect( cur_menu.pos.x, cur_menu.pos.y, cur_menu.pos.x + cur_menu.size.x, cur_menu.pos.y + cur_menu.size.y );
}

void gui::framework::end( ) {
	// draw scrollbar only if the content cant fit in to the page
	const auto max_y = cur_menu.cursor.y - cur_menu.size.y;
	if ( max_y > 0 ) {
		const vec2_t cur_pos{ cur_menu.pos.x + cur_menu.size.x - 14, cur_menu.pos.y + 8 };

		const auto   height = std::max( cur_menu.size.y - max_y, 16.f );
		const vec2_t size{ 8, cur_menu.size.y - height - 16 };

		bool hover    = !cur_menu.block_input && photon->input->is_cursor_in_area( cur_pos.x, cur_pos.y, cur_pos.x + size.x, cur_pos.y + cur_menu.size.y - 16 );
		bool clicking = photon->input->get_key_held( mouse_left );

#define SCROLLBAR_ID "photon scrollbar"

		static int grab_pos{ };

		float value = ( float ) scroll_offset / max_y;

		if ( hover && clicking && cur_slider.empty( ) ) {
			cur_slider = SCROLLBAR_ID;
			grab_pos   = photon->input->get_cursor_position( ).y - cur_pos.y - value * size.y;
		} else if ( !clicking && !cur_slider.empty( ) )
			cur_slider.clear( );

		if ( cur_slider == SCROLLBAR_ID ) {
			value         = ( photon->input->get_cursor_position( ).y - cur_pos.y - grab_pos ) / size.y;
			scroll_offset = std::clamp( value * max_y, 0.f, max_y );
			value         = ( float ) scroll_offset / max_y;
		}

		photon->render->draw_rounded_rect( cur_pos.x, cur_pos.y + value * size.y, 8, height, cur_slider == SCROLLBAR_ID ? colors::accent : colors::fg, 4 );

#undef SCROLLBAR_ID
	} else
		scroll_offset = 0;  // reset scroll offset if content can fit in page

	interfaces::surface->set_clip_rect( 0, 0, photon->render->get_screen_size( ).x, photon->render->get_screen_size( ).y );
	interfaces::surface->enable_clipping = false;

	// draw dropdowns
	if ( !cur_dropdown.id.empty( ) ) {
		auto cur_pos = cur_dropdown.pos;

		const vec2_t size{ 168, 28 };

		cur_pos.y += size.y;

		photon->render->draw_rounded_rect( cur_pos.x, cur_pos.y, size.x, cur_dropdown.items.size( ) * size.y, colors::accent, 8 );
		photon->render->draw_rounded_rect( cur_pos.x + 1, cur_pos.y + 1, size.x - 2, cur_dropdown.items.size( ) * size.y - 2, colors::bg, 8 );

		for ( std::size_t i = 0; i < cur_dropdown.items.size( ); ++i ) {
			const auto& item = cur_dropdown.items[ i ];

			bool hover    = photon->input->is_cursor_in_area( cur_pos.x + 1, cur_pos.y + 1, cur_pos.x + size.x - 1, cur_pos.y + size.y - 1 );
			bool clicking = hover && photon->input->get_key_press( mouse_left );

			if ( !cur_dropdown.multiselect ) {
				if ( clicking ) {
					cur_dropdown.value = i;
					cur_dropdown.done  = true;
				}

				photon->render->draw_text( cur_pos.x + 8, cur_pos.y + 6, fonts::smaller, hover ? colors::accent : colors::text, false, item.c_str( ) );
			} else {
				if ( clicking )
					cur_dropdown.value ^= ( 1 << i );

				photon->render->draw_text( cur_pos.x + 8, cur_pos.y + 6, fonts::smaller, cur_dropdown.value & ( 1 << i ) ? colors::accent : colors::text, false, item.c_str( ) );
			}

			cur_pos.y += size.y;
		}
	}

	// draw colorpickers
	if ( !cur_colorpicker.id.empty( ) ) {
		auto cur_pos = cur_colorpicker.pos;

		vec2_t size{ 220, 200 };

		cur_pos.x += 20;

		photon->render->draw_rounded_rect( cur_pos.x, cur_pos.y, size.x, size.y, colors::fg, 8 );
		photon->render->draw_rounded_rect( cur_pos.x + 1, cur_pos.y + 1, size.x - 2, size.y - 2, colors::bg, 8 );

		cur_pos += { 8, 8 };
		size -= { 16, 16 };

		const auto width  = size.x - 24;
		const auto height = size.y - 20;

		/* logic */
		bool hover_sv = photon->input->is_cursor_in_area( cur_pos.x, cur_pos.y, cur_pos.x + width - 2, cur_pos.y + height - 2 );
		bool hover_h  = photon->input->is_cursor_in_area( cur_pos.x + size.x - 18, cur_pos.y, cur_pos.x + size.x - 18 + 20, cur_pos.y + height - 2 );
		bool hover_a  = photon->input->is_cursor_in_area( cur_pos.x, cur_pos.y + size.y - 14, cur_pos.x + width - 2, cur_pos.y + size.y - 14 + 14 );
		bool holding  = photon->input->get_key_held( mouse_left );
		bool clicking = photon->input->get_key_press( mouse_left );

		float& h = cur_colorpicker.h;
		float& s = cur_colorpicker.s;
		float& v = cur_colorpicker.v;
		float& a = cur_colorpicker.a;

		// clicking out
		// HACK: check if not first frame by checking if alpha is initialized
		if ( a != -1 && clicking && !hover_sv && !hover_h && !hover_a ) {
			cur_colorpicker = { };
			return;
		}

		a = cur_colorpicker.value.a / 255.f;

#define SV_ID "photon colorpicker sv"
#define H_ID  "photon colorpicker h"
#define A_ID  "photon colorpicker a"

		if ( holding && cur_slider.empty( ) ) {
			if ( hover_sv )
				cur_slider = SV_ID;
			else if ( hover_h )
				cur_slider = H_ID;
			else if ( hover_a )
				cur_slider = A_ID;
		} else if ( !holding && !cur_slider.empty( ) )
			cur_slider.clear( );

		if ( cur_slider == SV_ID ) {
			s = std::clamp( ( photon->input->get_cursor_position( ).x - cur_pos.x ) / ( width - 2 ), 0.f, 1.f );
			v = 1.f - std::clamp( ( photon->input->get_cursor_position( ).y - cur_pos.y ) / ( height - 2 ), 0.f, 1.f );
		} else if ( cur_slider == H_ID )
			h = std::clamp( ( photon->input->get_cursor_position( ).y - cur_pos.y ) / ( height - 2 ), 0.f, 1.f );
		else if ( cur_slider == A_ID )
			a = std::clamp( ( photon->input->get_cursor_position( ).x - cur_pos.x ) / ( width - 2 ), 0.f, 1.f );

#undef SV_ID
#undef H_ID
#undef A_ID

		color_t col = color_t::from_hsv( h, s, v );
		col.a       = a * 255;

		cur_colorpicker.value = col;

		/* drawing */
		// saturation, value
		photon->render->draw_outlined_rect( cur_pos.x - 1, cur_pos.y - 1, width + 2, height + 2, colors::fg );
		photon->render->draw_gradient( cur_pos.x, cur_pos.y, width, height, { 255, 255, 255 }, color_t::from_hsv( h, 1.f, 1.f ), true );
		photon->render->draw_gradient( cur_pos.x, cur_pos.y, width, height, { 0, 0, 0, 0 }, { 0, 0, 0 }, false );

		// hue
		photon->render->draw_outlined_rect( cur_pos.x + size.x - 18 - 1, cur_pos.y - 1, 20, height + 2, colors::fg );
		photon->render->draw_texture( cur_pos.x + size.x - 18, cur_pos.y, 18, height, "photon_hue_gradient" );

		// alpha
		photon->render->draw_outlined_rect( cur_pos.x - 1, cur_pos.y + size.y - 14 - 1, width + 2, 16, colors::fg );
		photon->render->draw_filled_rect( cur_pos.x, cur_pos.y + size.y - 14, width, 14, col );

		/* draw cursors */
		// saturation, value
		photon->render->draw_outlined_rect( cur_pos.x + s * ( width - 2 ) - 1, cur_pos.y + ( 1.f - v ) * ( height - 2 ) - 1, 4, 4, colors::gray );
		photon->render->draw_filled_rect( cur_pos.x + s * ( width - 2 ), cur_pos.y + ( 1.f - v ) * ( height - 2 ), 2, 2, { 255, 255, 255, 64 } );

		// hue
		photon->render->draw_outlined_rect( cur_pos.x + size.x - 18 - 1, cur_pos.y + h * ( height - 2 ) - 1, 20, 3, colors::gray );
		photon->render->draw_filled_rect( cur_pos.x + size.x - 18, cur_pos.y + h * ( height - 2 ), 18, 1, { 255, 255, 255, 64 } );

		// alpha
		photon->render->draw_outlined_rect( cur_pos.x + a * ( width - 2 ) - 1, cur_pos.y + size.y - 14 - 1, 3, 16, colors::gray );
		photon->render->draw_filled_rect( cur_pos.x + a * ( width - 2 ), cur_pos.y + size.y - 14, 1, 14, { 255, 255, 255, 64 } );
	}
}

bool gui::framework::tab( int& selected, const vec2_t& pos, const vec2_t& size, const std::string& label, bool texture ) {
	bool hover  = photon->input->is_cursor_in_area( pos.x, pos.y, pos.x + size.x, pos.y + size.y );
	bool active = ( hover && photon->input->get_key_press( mouse_left ) ) || selected == cur_menu.tab_count;
	if ( active ) {
		selected = cur_menu.tab_count;

		cur_menu.cursor = { 8, 8 };
	}

	interfaces::surface->set_clip_rect( 0, 0, photon->render->get_screen_size( ).x, photon->render->get_screen_size( ).y );

	photon->render->draw_filled_rect( pos.x, pos.y, size.x, size.y, hover ? colors::accent : colors::bg );
	if ( hover )
		photon->render->draw_outlined_rect( pos.x - 1, pos.y - 1, size.x + 2, size.y + 2, colors::bg, 3 );

	if ( !texture ) {
		const auto text_size = photon->render->get_text_size( fonts::title, label.c_str( ) );
		photon->render->draw_text( pos.x + size.x / 2, pos.y + size.y / 2 - text_size.y / 2, fonts::title, colors::text, true, label.c_str( ) );
	} else {
		auto tex_size = size / 1.75f;
		photon->render->draw_texture( pos.x + size.x / 2 - tex_size.x / 2, pos.y + size.y / 2 - tex_size.y / 2, round( tex_size.x ), round( tex_size.y ), label.c_str( ), colors::text );
	}

	interfaces::surface->set_clip_rect( cur_menu.pos.x + 1, cur_menu.pos.y + 1, cur_menu.pos.x + cur_menu.size.x - 1, cur_menu.pos.y + cur_menu.size.y - 1 );

	++cur_menu.tab_count;

	return active;
}

bool gui::framework::mod( mods::mod_info_t& info ) {
	const vec2_t size{ 232, 158 };

	if ( cur_menu.mod_count % 3 == 0 ) {
		cur_menu.cursor.x = 8;
	} else {
		cur_menu.cursor.x += size.x;
		cur_menu.cursor.y -= 80 + 36 + 40 + 8;
	}

	auto cur_pos = cur_menu.pos + cur_menu.cursor;
	cur_pos.y -= scroll_offset;

	photon->render->draw_filled_rect( cur_pos.x, cur_pos.y, size.x, size.y, info.is_loaded ? colors::green : colors::fg );

	const auto mod_info = info.ptr->get_info( );

	photon->render->draw_text( cur_pos.x + size.x / 2 + 2, cur_pos.y + 16 + 2, fonts::normal, colors::gray, true, mod_info.name );
	photon->render->draw_text( cur_pos.x + size.x / 2, cur_pos.y + 16, fonts::normal, colors::white, true, mod_info.name );

	photon->render->draw_text( cur_pos.x + size.x / 2, cur_pos.y + 40, fonts::smaller, colors::text, true, util::ssprintf( "by %s", mod_info.author ).c_str( ) );

	cur_menu.cursor += { 8, 80 };

	if ( !info.is_loaded ) {
		if ( button( { size.x - 16, 28 }, "ENABLE", true, fonts::smaller, colors::green ) )
			mods::enable( &info );
	} else {
		if ( button( { size.x - 16, 28 }, "DISABLE", true, fonts::smaller, colors::red ) )
			mods::disable( &info );
	}

	bool result = button( { size.x - 16, 32 }, "OPTIONS", info.is_loaded );

	++cur_menu.mod_count;

	cur_menu.cursor.y += 8;

	return result;
}

bool gui::framework::config( const std::string& label ) {
	const vec2_t size{ cur_menu.size.x - 16, 36 };
	const vec2_t btn_size{ 28, 28 };

	auto cur_pos = cur_menu.pos + vec2_t{ 8, cur_menu.cursor.y - scroll_offset };

	photon->render->draw_rounded_rect( cur_pos.x, cur_pos.y, size.x, size.y, colors::fg, 8 );
	photon->render->draw_rounded_rect( cur_pos.x + 1, cur_pos.y + 1, size.x - 2, size.y - 2, colors::bg, 8 );

	photon->render->draw_text( cur_pos.x + 8, cur_pos.y + 8, fonts::normal, colors::text, false, label.c_str( ) );

	cur_menu.cursor.x = align_right( cur_pos, btn_size * 3 ).x - cur_menu.pos.x - 8;
	cur_menu.cursor.y += 4;

	if ( icon_button( btn_size, "photon_download", true ) )
		configs::save( label.c_str( ) );

	if ( icon_button( btn_size, "photon_upload", true ) )
		configs::load( label.c_str( ) );

	if ( icon_button( btn_size, "photon_trash_can", true ) )
		configs::remove( label.c_str( ) );

	cur_menu.cursor.x = 8;
	cur_menu.cursor.y += size.y + 4;

	return true;
}

bool gui::framework::icon_button( const vec2_t& size, const std::string& texture, bool same_line, const color_t& color ) {
	auto cur_pos = cur_menu.pos + cur_menu.cursor;
	cur_pos.y -= scroll_offset;

	bool hover    = !cur_menu.block_input && photon->input->is_cursor_in_area( cur_pos.x, cur_pos.y, cur_pos.x + size.x, cur_pos.y + size.y );
	bool clicking = hover && photon->input->get_key_press( mouse_left );

	photon->render->draw_filled_rect( cur_pos.x, cur_pos.y, size.x, size.y, hover ? colors::accent : color );
	if ( hover )
		photon->render->draw_outlined_rect( cur_pos.x - 1, cur_pos.y - 1, size.x + 2, size.y + 2, colors::bg, 3 );

	auto tex_size = size / 1.75f;
	photon->render->draw_texture( cur_pos.x + size.x / 2 - tex_size.x / 2, cur_pos.y + size.y / 2 - tex_size.y / 2, round( tex_size.x ), round( tex_size.y ), texture.c_str( ), colors::text );

	if ( !same_line )
		cur_menu.cursor.y += size.y + 8;
	else
		cur_menu.cursor.x += size.x + 4;

	return clicking;
}

void gui::framework::set_cursor( const vec2_t& pos ) {
	cur_menu.cursor = pos;
}

void gui::framework::split( int width ) {
	cur_menu.cursor.x += width + 8;
	cur_menu.cursor.y = 8;
}

bool gui::framework::button( const vec2_t& size, const std::string& label, bool enabled, h_font font, const color_t& color ) {
	auto cur_pos = cur_menu.pos + cur_menu.cursor;
	cur_pos.y -= scroll_offset;

	bool hover    = enabled && !cur_menu.block_input && photon->input->is_cursor_in_area( cur_pos.x, cur_pos.y, cur_pos.x + size.x, cur_pos.y + size.y );
	bool clicking = hover && photon->input->get_key_press( mouse_left );

	photon->render->draw_filled_rect( cur_pos.x, cur_pos.y, size.x, size.y, hover ? color : colors::bg );
	if ( hover )
		photon->render->draw_outlined_rect( cur_pos.x - 1, cur_pos.y - 1, size.x + 2, size.y + 2, colors::bg, 3 );

	const auto text_size = photon->render->get_text_size( font, label.c_str( ) );

	photon->render->draw_text( cur_pos.x + size.x / 2, cur_pos.y + size.y / 2 - text_size.y / 2, font, colors::text, true, label.c_str( ) );

	if ( !enabled )
		photon->render->draw_filled_rect( cur_pos.x, cur_pos.y, size.x, size.y, colors::disabled );

	cur_menu.cursor.y += size.y + 8;

	return clicking;
}

bool gui::framework::toggle( bool& val, const std::string& label ) {
	auto cur_pos = cur_menu.pos + cur_menu.cursor;
	cur_pos.y -= scroll_offset;

	const vec2_t size{ 40, 20 };
	const int    radius      = size.y / 2 - 2;
	const int    radius_half = radius / 2;

	auto cur_pos2 = align_right( cur_pos, size );

	bool hover  = !cur_menu.block_input && photon->input->is_cursor_in_area( cur_pos2.x, cur_pos2.y, cur_pos2.x + size.x, cur_pos2.y + size.y );
	bool result = false;

	if ( hover && photon->input->get_key_press( mouse_left ) ) {
		val    = !val;
		result = true;
	}

	photon->render->draw_text( cur_pos.x, cur_pos.y, fonts::normal, colors::text, false, label.c_str( ) );

	photon->render->draw_rounded_rect( cur_pos2.x, cur_pos2.y, size.x, size.y, val ? colors::accent : colors::fg, 10 );

	cur_pos2 += { radius, radius };
	cur_pos2.x += val * size.x / 2;

	photon->render->draw_circle( cur_pos2.x + 2, cur_pos2.y + 2, radius, colors::white );

	cur_menu.cursor.y += size.y + 16;

	return result;
}

void gui::framework::slider( int& val, int min, int max, const std::string& label ) {
	auto cur_pos = cur_menu.pos + cur_menu.cursor;
	cur_pos.y -= scroll_offset;

	const vec2_t size{ 200, 20 };
	const int    radius      = size.y / 2;
	const int    radius_half = radius / 2;

	auto cur_pos2 = align_right( cur_pos, size );

	bool hover    = !cur_menu.block_input && photon->input->is_cursor_in_area( cur_pos2.x, cur_pos2.y, cur_pos2.x + size.x, cur_pos2.y + size.y );
	bool clicking = photon->input->get_key_held( mouse_left );

	if ( hover && clicking && cur_slider.empty( ) )
		cur_slider = label;
	else if ( !clicking && !cur_slider.empty( ) )
		cur_slider.clear( );

	float value = ( float ) val / ( max - min );

	if ( cur_slider == label ) {
		value = ( photon->input->get_cursor_position( ).x - cur_pos2.x ) / size.x;
		val   = std::clamp( value * ( max - min ), ( float ) min, ( float ) max );
		value = ( float ) val / ( max - min );
	}

	photon->render->draw_text( cur_pos.x, cur_pos.y, fonts::normal, colors::text, false, label.c_str( ) );

	cur_pos2.y += 6;

	photon->render->draw_rounded_rect( cur_pos2.x, cur_pos2.y + 1, size.x, size.y - 13, colors::fg, 4 );
	photon->render->draw_rounded_rect( cur_pos2.x, cur_pos2.y + 1, value * size.x, size.y - 13, colors::accent, 4 );

	photon->render->draw_text( cur_pos2.x - 12, cur_pos2.y - 4, fonts::smaller, colors::text, 2, util::ssprintf( "%d", val ).c_str( ) );

	cur_pos2 += { radius_half, radius_half };
	cur_pos2.x += value * ( size.x - radius );

	photon->render->draw_circle( cur_pos2.x, cur_pos2.y, radius, colors::accent );
	photon->render->draw_circle( cur_pos2.x, cur_pos2.y, radius - 4, colors::white );

	cur_menu.cursor.y += size.y + 16;
}

void gui::framework::sliderf( float& val, float min, float max, const std::string& label ) {
	auto cur_pos = cur_menu.pos + cur_menu.cursor;
	cur_pos.y -= scroll_offset;

	const vec2_t size{ 200, 20 };
	const int    radius      = size.y / 2;
	const int    radius_half = radius / 2;

	auto cur_pos2 = align_right( cur_pos, size );

	bool hover    = !cur_menu.block_input && photon->input->is_cursor_in_area( cur_pos2.x, cur_pos2.y, cur_pos2.x + size.x, cur_pos2.y + size.y );
	bool clicking = photon->input->get_key_held( mouse_left );

	if ( hover && clicking && cur_slider.empty( ) )
		cur_slider = label;
	else if ( !clicking && !cur_slider.empty( ) )
		cur_slider.clear( );

	float value = val / ( max - min );

	if ( cur_slider == label ) {
		value = ( photon->input->get_cursor_position( ).x - cur_pos2.x ) / size.x;
		val   = std::clamp( value * ( max - min ), min, max );
		value = val / ( max - min );
	}

	photon->render->draw_text( cur_pos.x, cur_pos.y, fonts::normal, colors::text, false, label.c_str( ) );

	cur_pos2.y += 6;

	photon->render->draw_rounded_rect( cur_pos2.x, cur_pos2.y + 1, size.x, size.y - 13, colors::fg, 4 );
	photon->render->draw_rounded_rect( cur_pos2.x, cur_pos2.y + 1, value * size.x, size.y - 13, colors::accent, 4 );

	photon->render->draw_text( cur_pos2.x - 12, cur_pos2.y - 4, fonts::smaller, colors::text, 2, util::ssprintf( "%.1f", val ).c_str( ) );

	cur_pos2 += { radius_half, radius_half };
	cur_pos2.x += value * ( size.x - radius );

	photon->render->draw_circle( cur_pos2.x, cur_pos2.y, radius, colors::accent );
	photon->render->draw_circle( cur_pos2.x, cur_pos2.y, radius - 4, colors::white );

	cur_menu.cursor.y += size.y + 16;
}

void gui::framework::colorpicker( color_t& val, const std::string& label ) {
	auto cur_pos = cur_menu.pos + cur_menu.cursor;
	cur_pos.y -= scroll_offset;

	const vec2_t size{ 20, 20 };

	auto cur_pos2 = align_right( cur_pos, size );

	bool open = cur_colorpicker.id == label;

	bool hover    = ( !cur_menu.block_input || open ) && photon->input->is_cursor_in_area( cur_pos2.x, cur_pos2.y, cur_pos2.x + size.x, cur_pos2.y + size.y );
	bool clicking = hover && photon->input->get_key_press( mouse_left );

	if ( clicking ) {
		if ( open )
			cur_colorpicker = { };
		else {
			cur_colorpicker.id    = label;
			cur_colorpicker.pos   = cur_pos2;
			cur_colorpicker.value = val;
			val.to_hsv( cur_colorpicker.h, cur_colorpicker.s, cur_colorpicker.v );
		}

		open = !open;
	}

	if ( open )
		val = cur_colorpicker.value;

	photon->render->draw_text( cur_pos.x, cur_pos.y, fonts::normal, colors::text, false, label.c_str( ) );

	photon->render->draw_rounded_rect( cur_pos2.x, cur_pos2.y, size.x, size.y, colors::gray, 8 );

	photon->render->draw_rounded_rect( cur_pos2.x + 1, cur_pos2.y + 1, size.x - 2, size.y - 2, colors::bg, 8 );
	photon->render->draw_rounded_rect( cur_pos2.x + 1, cur_pos2.y + 1, size.x - 2, size.y - 2, val, 8 );

	cur_menu.cursor.y += size.y + 16;
}

void gui::framework::combo( std::size_t& val, const std::vector< std::string >& items, const std::string& label ) {
	auto cur_pos = cur_menu.pos + cur_menu.cursor;
	cur_pos.y -= scroll_offset;

	const vec2_t size{ 168, 28 };

	auto cur_pos2 = align_right( cur_pos, size );
	cur_pos2.y -= 3;

	bool open = cur_dropdown.id == label;

	bool hover    = ( !cur_menu.block_input || open ) && photon->input->is_cursor_in_area( cur_pos2.x, cur_pos2.y, cur_pos2.x + size.x, cur_pos2.y + size.y );
	bool clicking = hover && photon->input->get_key_press( mouse_left );

	if ( clicking ) {
		if ( open )
			cur_dropdown = { };
		else {
			cur_dropdown.id          = label;
			cur_dropdown.pos         = cur_pos2;
			cur_dropdown.items       = items;
			cur_dropdown.value       = val;
			cur_dropdown.multiselect = false;
		}

		open = !open;
	}

	if ( cur_dropdown.done ) {
		val = cur_dropdown.value;

		cur_dropdown = { };
	}

	photon->render->draw_text( cur_pos.x, cur_pos.y, fonts::normal, colors::text, false, label.c_str( ) );

	photon->render->draw_rounded_rect( cur_pos2.x, cur_pos2.y, size.x, size.y, open ? colors::accent : colors::fg, 8 );
	photon->render->draw_rounded_rect( cur_pos2.x + 1, cur_pos2.y + 1, size.x - 2, size.y - 2, colors::bg, 8 );
	photon->render->draw_texture( cur_pos2.x + size.x - 20, cur_pos2.y + 10, 8, 8, open ? "photon_caret_up" : "photon_caret_down", colors::fg );

	interfaces::surface->set_clip_rect( cur_pos2.x + 8, cur_pos2.y, cur_pos2.x + size.x - 20, cur_pos2.y + size.y );
	photon->render->draw_text( cur_pos2.x + 8, cur_pos2.y + 6, fonts::smaller, colors::text, false, items[ val ].c_str( ) );
	interfaces::surface->set_clip_rect( cur_menu.pos.x, cur_menu.pos.y, cur_menu.pos.x + cur_menu.size.x, cur_menu.pos.y + cur_menu.size.y );

	photon->render->draw_gradient( cur_pos2.x + size.x - 52, cur_pos2.y + 1, 32, size.y - 2, { 0, 0, 0, 0 }, colors::bg, true );

	cur_menu.cursor.y += size.y + 16;
}

void gui::framework::multicombo( std::size_t& val, const std::vector< std::string >& items, const std::string& label ) {
	auto cur_pos = cur_menu.pos + cur_menu.cursor;
	cur_pos.y -= scroll_offset;

	const vec2_t size{ 168, 28 };

	auto cur_pos2 = align_right( cur_pos, size );
	cur_pos2.y -= 3;

	bool open = cur_dropdown.id == label;

	bool hover    = ( !cur_menu.block_input || open ) && photon->input->is_cursor_in_area( cur_pos2.x, cur_pos2.y, cur_pos2.x + size.x, cur_pos2.y + size.y );
	bool clicking = hover && photon->input->get_key_press( mouse_left );

	if ( clicking ) {
		if ( open ) {
			val = cur_dropdown.value;

			cur_dropdown = { };
		} else {
			cur_dropdown.id          = label;
			cur_dropdown.pos         = cur_pos2;
			cur_dropdown.items       = items;
			cur_dropdown.value       = val;
			cur_dropdown.multiselect = true;
		}

		open = !open;
	}

	if ( open )
		val = cur_dropdown.value;

	photon->render->draw_text( cur_pos.x, cur_pos.y, fonts::normal, colors::text, false, label.c_str( ) );

	photon->render->draw_rounded_rect( cur_pos2.x, cur_pos2.y, size.x, size.y, open ? colors::accent : colors::fg, 8 );
	photon->render->draw_rounded_rect( cur_pos2.x + 1, cur_pos2.y + 1, size.x - 2, size.y - 2, colors::bg, 8 );
	photon->render->draw_texture( cur_pos2.x + size.x - 20, cur_pos2.y + 10, 8, 8, open ? "photon_caret_up" : "photon_caret_down", colors::fg );

	std::string display_text;
	bool        comma = false;
	for ( std::size_t i = 0; i < items.size( ); ++i ) {
		if ( val & ( 1 << i ) ) {
			display_text += ( comma ? ", " : "" ) + items[ i ];

			comma = true;
		}
	}

	interfaces::surface->set_clip_rect( cur_pos2.x + 8, cur_pos2.y, cur_pos2.x + size.x - 20, cur_pos2.y + size.y );
	photon->render->draw_text( cur_pos2.x + 8, cur_pos2.y + 6, fonts::smaller, colors::text, false, display_text.c_str( ) );
	interfaces::surface->set_clip_rect( cur_menu.pos.x, cur_menu.pos.y, cur_menu.pos.x + cur_menu.size.x, cur_menu.pos.y + cur_menu.size.y );

	photon->render->draw_gradient( cur_pos2.x + size.x - 52, cur_pos2.y + 1, 32, size.y - 2, { 0, 0, 0, 0 }, colors::bg, true );

	cur_menu.cursor.y += size.y + 16;
}

void gui::framework::textbox( const char*& val, const std::string& label ) {
	auto cur_pos = cur_menu.pos + cur_menu.cursor;
	cur_pos.y -= scroll_offset;

	const vec2_t size{ 168, 28 };

	bool open = cur_textbox == label;

	bool hover    = !cur_menu.block_input && photon->input->is_cursor_in_area( cur_pos.x, cur_pos.y, cur_pos.x + size.x, cur_pos.y + size.y );
	bool clicking = photon->input->get_key_press( mouse_left );

	if ( hover && clicking ) {
		open = !open;
		if ( open )
			cur_textbox = label;
		else
			cur_textbox.clear( );
	} else if ( !hover && clicking ) {
		open = false;
		cur_textbox.clear( );
	}

	std::string s{ val };

	if ( open ) {
		// FIXME: impl holding keys

		if ( photon->input->get_key_press( key_space ) )
			s += " ";

		if ( photon->input->get_key_press( key_backspace ) && !s.empty( ) )
			s.pop_back( );

		bool upper = photon->input->get_key_held( key_lshift ) || photon->input->get_key_held( key_rshift );

		// go through actual characters
		for ( std::size_t i = key_0; i < key_pad_9; ++i ) {
			if ( photon->input->get_key_press( ( e_button_code ) i ) ) {
				char c = *interfaces::input_system->button_code_to_string( ( e_button_code ) i );
				s += upper ? toupper( c ) : c;
			}
		}

		if ( photon->input->get_key_press( key_enter ) || photon->input->get_key_press( key_escape ) )
			cur_textbox.clear( );

		val = strdup( s.c_str( ) );
	}

	photon->render->draw_rounded_rect( cur_pos.x, cur_pos.y, size.x, size.y, open ? colors::accent : colors::fg, 8 );
	photon->render->draw_rounded_rect( cur_pos.x + 1, cur_pos.y + 1, size.x - 2, size.y - 2, colors::bg, 8 );

	if ( s.empty( ) )
		photon->render->draw_text( cur_pos.x + 8, cur_pos.y + 6, fonts::smaller, colors::fg, false, label.c_str( ) );
	else {
		const auto text_size = photon->render->get_text_size( fonts::smaller, val );

		auto offset = text_size.x > size.x ? text_size.x - size.x : 0;

		interfaces::surface->set_clip_rect( cur_pos.x + 8, cur_pos.y, cur_pos.x + size.x - 8, cur_pos.y + size.y );
		photon->render->draw_text( cur_pos.x + 8 - offset, cur_pos.y + 6, fonts::smaller, colors::text, false, val );
		interfaces::surface->set_clip_rect( cur_menu.pos.x, cur_menu.pos.y, cur_menu.pos.x + cur_menu.size.x, cur_menu.pos.y + cur_menu.size.y );
	}

	cur_menu.cursor.y += size.y + 16;
}

void gui::framework::separator( const std::string& label ) {
	auto cur_pos = cur_menu.pos + cur_menu.cursor;
	cur_pos.y -= scroll_offset;

	const vec2_t size = { cur_menu.size.x - cur_menu.cursor.x - 20, 2 };

	const auto text_size = photon->render->get_text_size( fonts::normal, label.c_str( ) );

	photon->render->draw_text( cur_pos.x, cur_pos.y, fonts::normal, colors::text, false, label.c_str( ) );

	photon->render->draw_filled_rect( cur_pos.x, cur_pos.y + text_size.y, size.x, size.y, colors::disabled );

	cur_menu.cursor.y += size.y + text_size.y + 8;
}
