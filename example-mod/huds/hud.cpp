#include "hud.h"

#include <photon.h>

c_example_hud* ex_hud;

bool huds::initialize( ) {
	photon::get( )->hud->reg( "example_hud", ex_hud = new c_example_hud( ) );

	return true;
}

void huds::uninitialize( ) {
	photon::get( )->hud->unreg( "example_hud" );
}

void c_example_hud::draw( ) {
	vertex_t verts[]{
		{ { 100, 0 } },
		{ { 300, 300 } },
		{ { 0, 200 } }
	};
	photon::get( )->hud->draw_polygon( 3, verts, { } );
}
