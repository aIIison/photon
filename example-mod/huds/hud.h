#pragma once

#include <photon.h>

namespace huds {
	bool initialize( );
	void uninitialize( );
}  // namespace huds

class c_example_hud : public photon::i_hud {
	virtual void draw( );
	virtual bool is_splitscreen( ) { return true; };
};
