#include "mod.h"

#include "config/config.h"
#include "convars/convars.h"
#include "huds/hud.h"
#include "signals/signals.h"

EXPOSE_PHOTON_MOD( c_example_mod );

h_font font;

bool c_example_mod::load( ) {
	if ( !signals::initialize( ) )
		return false;

	if ( !convars::initialize( ) )
		return false;

	if ( !huds::initialize( ) )
		return false;

	if ( !config::initialize( ) )
		return false;

	photon::get( )->render->create_font( font, "Tahoma", 12, false, fontflag_dropshadow );

	return true;
}

void c_example_mod::unload( ) {
	photon::get( )->render->destruct_font( font );

	config::uninitialize( );
	huds::uninitialize( );
	convars::uninitialize( );
	signals::uninitialize( );
}

void c_example_mod::on_event( const char* msg ) {
	if ( !strcmp( msg, "paint" ) ) {
		photon::get( )->render->draw_text( 2, 2, font, { }, false, "example mod loaded." );
	}
	if ( !strcmp( msg, "on_screen_size_changed" ) ) {
		photon::get( )->render->create_font( font, "Tahoma", 12, false, fontflag_dropshadow );
	}
}

photon::mod_info_t c_example_mod::get_info( ) {
	return {
		.name    = "Example Mod",
		.author  = "hero",
		.version = "0.0.1"
	};
}

void c_example_mod::paint_menu( photon::c_menu_context* ctx ) {
	ctx->toggle( config::example_boolean, "Example Toggle" );
	ctx->slider( config::example_integer, 0, 100, "Example Slider" );
	ctx->sliderf( config::example_float, 0.f, 10.f, "Example Sliderf" );
	ctx->colorpicker( config::example_color, "Example Colorpicker" );

	ctx->separator( "Example Separator" );

	static const char* values[] = { "Value 1", "Value 2", "Value 3" };
	ctx->combo( config::example_combo_value, values, 3, "Example Combo" );
	ctx->multicombo( config::example_mcombo_value, values, 3, "Example Multicombo" );
	ctx->textbox( config::example_textbox_value, "Example Textbox" );
}
