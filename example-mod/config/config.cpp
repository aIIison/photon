#include "config.h"

bool config::initialize( ) {
	photon->config->begin( "example-mod" );
	photon->config->serialize( "example_boolean", example_boolean );
	photon->config->serialize( "example_integer", example_integer );
	photon->config->serialize( "example_float", example_float );
	photon->config->serialize( "example_color", example_color );
	photon->config->serialize( "example_combo_value", example_combo_value );
	photon->config->serialize( "example_mcombo_value", example_mcombo_value );
	photon->config->serialize( "example_textbox_value", example_textbox_value );
	photon->config->end( );

	return true;
}

void config::uninitialize( ) {
	photon->config->deserialize( "example-mod" );
}
