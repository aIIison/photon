#include "config.h"

bool config::initialize( ) {
	photon::get( )->config->begin( "example-mod" );
	photon::get( )->config->serialize( "example_boolean", example_boolean );
	photon::get( )->config->serialize( "example_integer", example_integer );
	photon::get( )->config->serialize( "example_float", example_float );
	photon::get( )->config->serialize( "example_color", example_color );
	photon::get( )->config->serialize( "example_combo_value", example_combo_value );
	photon::get( )->config->serialize( "example_mcombo_value", example_mcombo_value );
	photon::get( )->config->serialize( "example_textbox_value", example_textbox_value );
	photon::get( )->config->end( );

	return true;
}

void config::uninitialize( ) {
	photon::get( )->config->deserialize( "example-mod" );
}
