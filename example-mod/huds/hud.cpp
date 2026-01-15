#include "hud.h"

#include <photon.h>

c_example_hud* ex_hud;

bool huds::initialize( ) {
	photon->hud->reg( "example_hud", ex_hud = new c_example_hud( ) );

	return true;
}

void huds::uninitialize( ) {
	photon->hud->unreg( "example_hud" );
}

void c_example_hud::draw( ) {
	photon->hud->draw_begin( this );

	vertex_t verts[]{
		{ { 100, 0 } },
		{ { 300, 300 } },
		{ { 0, 200 } }
	};
	photon->hud->draw_polygon( 3, verts, { } );

	photon->hud->draw_end( );
}
