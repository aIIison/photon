#include "gui.h"

#include "../resource/resource.h"
#include "core/configs/configs.h"
#include "core/convars/convars.h"
#include "core/huds/huds.h"
#include "core/interfaces/interfaces.h"
#include "core/mods/mods.h"
#include "core/photon.h"
#include "framework.h"

// unlock the cursor from the game when menu is open
e_return_action lock_cursor( e_callback_type type, signal_context_t* ctx ) {
	static void* input_ctx = interfaces::engine_client->get_input_context( 0 );

	if ( gui::open || huds::edit ) {
		interfaces::surface->unlock_cursor( );

		interfaces::input_stack_system->set_cursor_visible( input_ctx, true );

		return e_return_action::Supercede;
	} else {
		interfaces::input_stack_system->set_cursor_visible( input_ctx, false );

		return e_return_action::Handled;
	}
}

e_return_action paint( e_callback_type type, signal_context_t* ctx ) {
	auto paint_mode = ( int ) photon->signal->get_arg( ctx, 1 );

	interfaces::surface->start_drawing( );

	if ( paint_mode == paint_uipanels ) {
		photon->input->poll_input( );  // not sure if this is the best place to call this

		huds::draw( );

		photon->common->post_event( &plugin, "paint" );

		if ( !interfaces::engine_client->con_is_visible( ) && photon->input->get_key_press( key_rshift ) ) {
			gui::open  = !gui::open;
			huds::edit = false;
		}

		if ( huds::edit && photon->input->get_key_press( key_escape ) ) {
			gui::open  = true;
			huds::edit = false;
		}

		if ( gui::open )
			gui::draw( );

		if ( huds::edit )
			huds::draw_ui( );
	}

	interfaces::surface->finish_drawing( );

	return e_return_action::Handled;
}

// block input to the game when photon's menu is open, only works in game, not in the menu
e_return_action in_key_event( e_callback_type type, signal_context_t* ctx ) {
	if ( gui::open || huds::edit ) {
		photon->signal->set_return( ctx, 0 );

		return e_return_action::Supercede;
	}

	return e_return_action::Ignored;
}

// block input to the menu, vgui has its own input system for some reason, so we have to hook another function
e_return_action update_button_state( e_callback_type type, signal_context_t* ctx ) {
	void* ecx = photon->signal->get_arg( ctx, 0 );

	if ( gui::open || huds::edit ) {
		/*
		 * so we cant actually just return here because theres other
		 * functions calling SetKeyCodeState and SetMouseCodeState,
		 * i didnt want to hook those functions so we just reset
		 * the struct that those functions update here
		 */
		int context = address_t( ecx ).at< int >( 0xce8 );  // m_hContext

		util::call_virtual< OS( 88, 89 ), void >( ecx, context );  // ResetInputContext
		return e_return_action::Supercede;
	}

	return e_return_action::Ignored;
}

static color_t* hue_tex;

void gui::create_fonts( ) {
	photon->render->create_font( framework::fonts::smaller, "Roboto Light", 18, true, fontflag_antialias );
	photon->render->create_font( framework::fonts::normal, "Roboto Light", 20, true, fontflag_antialias );
	photon->render->create_font( framework::fonts::title, "Roboto Light", 24, true, fontflag_antialias );
	photon->render->create_font( framework::fonts::bigtitle, "Roboto Light", 28, true, fontflag_antialias );
}

bool gui::initialize( ) {
	framework::set_theme( false );

	gui::create_fonts( );

	photon->render->load_texture_png( "photon_icon", resource::icons::photon, 50, 50, sizeof( resource::icons::photon ) );
	photon->render->load_texture_png( "photon_list", resource::icons::list, 32, 32, sizeof( resource::icons::list ) );
	photon->render->load_texture_png( "photon_gear", resource::icons::gear, 32, 32, sizeof( resource::icons::gear ) );
	photon->render->load_texture_png( "photon_left_arrow", resource::icons::left_arrow, 32, 32, sizeof( resource::icons::left_arrow ) );
	photon->render->load_texture_png( "photon_arrows", resource::icons::arrows, 32, 32, sizeof( resource::icons::arrows ) );
	photon->render->load_texture_png( "photon_download", resource::icons::download, 16, 16, sizeof( resource::icons::download ) );
	photon->render->load_texture_png( "photon_upload", resource::icons::upload, 16, 16, sizeof( resource::icons::upload ) );
	photon->render->load_texture_png( "photon_trash_can", resource::icons::trash_can, 16, 16, sizeof( resource::icons::trash_can ) );
	photon->render->load_texture_png( "photon_plus", resource::icons::plus, 16, 16, sizeof( resource::icons::plus ) );
	photon->render->load_texture_png( "photon_refresh", resource::icons::refresh, 16, 16, sizeof( resource::icons::refresh ) );
	photon->render->load_texture_png( "photon_caret_down", resource::icons::caret_down, 8, 8, sizeof( resource::icons::caret_down ) );
	photon->render->load_texture_png( "photon_caret_up", resource::icons::caret_up, 8, 8, sizeof( resource::icons::caret_up ) );

	// initialize hue gradient texture (this should probably be in framework)
	constexpr int tex_height = 180;

	hue_tex = new color_t[ 1 * tex_height ];
	for ( int i = 0; i < tex_height; ++i ) {
		float hue = i * ( 1.f / tex_height );

		hue_tex[ i ] = color_t::from_hsv( hue, 1.f, 1.f );
	}
	photon->render->load_texture_raw( "photon_hue_gradient", ( uint8_t* ) hue_tex, 1, tex_height );

	photon->signal->get( "CMatSystemSurface::LockCursor" )->add_callback( e_callback_type::Pre, &lock_cursor );
	photon->signal->get( "CEngineVGui::Paint" )->add_callback( e_callback_type::Post, &paint );
	photon->signal->get( "CHLClient::IN_KeyEvent" )->add_callback( e_callback_type::Pre, &in_key_event );
	photon->signal->get( "CEngineVGui::UpdateButtonState" )->add_callback( e_callback_type::Pre, &update_button_state );

	return true;
}

void gui::uninitialize( ) {
	SAFE_DELETE( hue_tex );

	photon->render->destruct_font( framework::fonts::bigtitle );
	photon->render->destruct_font( framework::fonts::title );
	photon->render->destruct_font( framework::fonts::normal );
	photon->render->destruct_font( framework::fonts::smaller );
}

void gui::draw( ) {
	const auto screen_size = photon->render->get_screen_size( );
	const auto screen_half = screen_size / 2;

	constexpr int tab_height = 56;
	const auto    menu_size  = vec2_t( 740, 0.4f * screen_size.y );
	const auto    menu_pos   = vec2_t( screen_half.x - menu_size.x / 2, screen_half.y + tab_height / 2 + 12 );

	static int tab = 1;

	static photon_api::i_photon_mod* cur_mod;

	// draw bg
	photon->render->draw_gradient( 0, 0, screen_size.x, screen_size.y, { 0, 0, 0, 200 }, { 32, 32, 32, 200 }, false );

	// draw title
	const auto title_size = photon->render->get_text_size( framework::fonts::bigtitle, "PHOTON" );
	photon->render->draw_texture( screen_half.x - title_size.x / 2 - 25 - 4, screen_half.y - 90, 50, 50, "photon_icon" );
	photon->render->draw_text( screen_half.x + 25 + 4, screen_half.y - 80, framework::fonts::bigtitle, framework::colors::white, true, "PHOTON" );

	framework::begin( menu_pos, menu_size );

	if ( framework::tab( tab, { screen_half.x - 100 - tab_height - 8, screen_half.y - tab_height / 2 }, { tab_height, tab_height }, "photon_list", true ) ) {
		if ( framework::icon_button( { 28, 28 }, "photon_refresh", true, framework::colors::fg ) )
			configs::iterate_cfgs( );

		static const char* cfg_name = "";
		if ( framework::icon_button( { 28, 28 }, "photon_plus", true, framework::colors::fg ) && !std::string( cfg_name ).empty( ) )
			configs::create( cfg_name );

		framework::textbox( cfg_name, "Config Name" );

		framework::set_cursor( { 8, 44 } );

		for ( auto& cfg : configs::cfgs ) {
			framework::config( cfg );
		}
	}
	if ( framework::tab( tab, { screen_half.x - 100, screen_half.y - tab_height / 2 }, { 200, tab_height }, "MODS" ) ) {
		if ( !cur_mod ) {
			for ( auto& mod : mods::mod_list ) {
				if ( framework::mod( mod.second ) )
					cur_mod = mod.second.ptr;
			}
		} else {
			if ( framework::icon_button( { 56, 56 }, "photon_left_arrow" ) )
				cur_mod = nullptr;

			if ( framework::icon_button( { 56, 56 }, "photon_arrows" ) ) {
				gui::open  = false;
				huds::edit = true;
			}

			framework::split( 56 );

			if ( cur_mod ) {
				framework::separator( util::ssprintf( "%s Settings", cur_mod->get_info( ).name ).c_str( ) );

				cur_mod->paint_menu( );
			}
		}
	}
	if ( framework::tab( tab, { screen_half.x + 100 + 8, screen_half.y - tab_height / 2 }, { tab_height, tab_height }, "photon_gear", true ) ) {
		framework::separator( "Menu Settings" );

		framework::colorpicker( framework::colors::accent, "Accent Color" );

		static bool dark_mode = false;
		if ( framework::toggle( dark_mode, "Dark Mode" ) )
			framework::set_theme( dark_mode );

		framework::separator( "Game Settings" );

		static bool fast_loads = true;
		if ( framework::toggle( fast_loads, "Fast Loads" ) )
			convars::set_fast_loads( fast_loads );

		framework::separator( "Hud Settings" );

		framework::slider( huds::safezone_x, 0, 32, "Hud Safezone X" );
		framework::slider( huds::safezone_y, 0, 32, "Hud Safezone Y" );
	}

	framework::end( );
}
