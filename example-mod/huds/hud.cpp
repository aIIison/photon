#include "hud.h"

#include <photon.h>

c_example_hud* ex_hud;

bool huds::initialize( ) {
	photon->hud->reg( ex_hud = new c_example_hud( ) );

	return true;
}

void huds::uninitialize( ) {
	photon->hud->unreg( ex_hud );
}

void c_example_hud::draw( ) {
	photon->hud->draw_begin( this );

	photon->hud->draw_filled_rect( 0, 0, 20, 20, { 255, 0, 0, 255 } );
	photon->hud->draw_filled_rect( 20, 20, 20, 20, { 0, 255, 0, 255 } );
	photon->hud->draw_filled_rect( 40, 40, 20, 20, { 0, 0, 255, 255 } );
	photon->hud->draw_filled_rect( 0, 60, 80, 20, { 255, 255, 255, 255 } );

	photon->hud->draw_end( );
}
