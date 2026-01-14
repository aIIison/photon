#pragma once

#include "core/mods/mods.h"
#include "sdk/photon.h"

#include <string>

namespace gui {
	namespace framework {
		namespace colors {
			// constant colors
			inline color_t white{ 255, 255, 255 };
			inline color_t gray{ 64, 64, 64 };
			inline color_t green{ 36, 140, 45 };
			inline color_t red{ 168, 43, 43 };

			// theme colors
			inline color_t accent{ 66, 128, 244, 255 };
			inline color_t bg;
			inline color_t fg;
			inline color_t text;
			inline color_t disabled;
		}  // namespace colors

		namespace fonts {
			inline h_font smaller;
			inline h_font normal;
			inline h_font title;
			inline h_font bigtitle;
		}  // namespace fonts

		struct menu_t {
			vec2_t pos;
			vec2_t size;

			vec2_t cursor;

			std::size_t tab_count;
			std::size_t mod_count;

			bool block_input;
		};
		inline menu_t cur_menu;

		struct dropdown_t {
			std::string id;

			vec2_t pos;

			std::vector< std::string > items;
			std::size_t                value;  // this is a bitmask when the dropdown is a multiselect and just a regular index when it isnt

			bool multiselect;
			bool done;
		};
		inline dropdown_t cur_dropdown;

		struct colorpicker_t {
			std::string id;
			vec2_t      pos;
			color_t     value;
			float       h, s, v;
			float       a{ -1 };
		};
		inline colorpicker_t cur_colorpicker;

		inline std::string cur_slider;
		inline std::string cur_textbox;

		inline int scroll_offset;  // FIXME: make this somehow be only for current menu but still persist

		void set_theme( bool dark = false );

		void begin( const vec2_t& pos, const vec2_t& size );
		void end( );
		bool tab( int& selected, const vec2_t& pos, const vec2_t& size, const std::string& label, bool use_texture = false );
		bool mod( mods::mod_info_t& info );
		bool config( const std::string& label );
		bool icon_button( const vec2_t& size, const std::string& texture, bool same_line = false, const color_t& color = colors::bg );
		void set_cursor( const vec2_t& pos );
		void split( int width );

		bool button( const vec2_t& size, const std::string& label, bool enabled = true, h_font font = fonts::normal, const color_t& color = colors::accent );
		bool toggle( bool& val, const std::string& label );
		void slider( int& val, int min, int max, const std::string& label );
		void sliderf( float& val, float min, float max, const std::string& label );
		void colorpicker( color_t& val, const std::string& label );
		void combo( std::size_t& val, const std::vector< std::string >& items, const std::string& label );
		void multicombo( std::size_t& val, const std::vector< std::string >& items, const std::string& label );
		void textbox( const char*& val, const std::string& label );
		void separator( const std::string& label );
	}  // namespace framework
}  // namespace gui
