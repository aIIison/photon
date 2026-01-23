#pragma once

#include <photon.h>

class c_example_mod : public photon::i_mod {
public:
	virtual bool               load( );
	virtual void               unload( );
	virtual void               on_event( const char* msg );
	virtual photon::mod_info_t get_info( );
	virtual void               paint_menu( photon::c_menu_context* ctx );
};
