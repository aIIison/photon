#pragma once

#include "ccon.h"
#include "common.h"
#include "config.h"
#include "hud.h"
#include "input.h"
#include "menu.h"
#include "platform.h"
#include "render.h"
#include "signal.h"
#include "source_sdk.h"

namespace photon {
	class c_shared {
	public:
		c_common* common;  // common functions.
		c_con*    con;     // create convars and concommands.
		c_signal* signal;  // photon's callback based hooking system.
		c_hud*    hud;     // register huds and thuds.
		c_render* render;  // wrapper around source's surface interface.
		c_input*  input;   // wrapper around source's inputsystem interface.
		c_config* config;  // photon's config system.
	};

	struct mod_info_t {
		const char* name;     // name of mod.
		const char* author;   // author of mod.
		const char* version;  // version of mod using semantic versioning.
	};

	class i_mod {
	public:
		virtual bool       load( )                           = 0;  // called on mod load, return if load was successful or not.
		virtual void       unload( )                         = 0;  // called on mod unload.
		virtual void       on_event( const char* msg )       = 0;  // callback for all events, `msg` contains event name.
		virtual mod_info_t get_info( )                       = 0;  // return mod info here.
		virtual void       paint_menu( c_menu_context* ctx ) = 0;  // called every frame when mod's settings menu is shown.
	};

	typedef c_shared* ( *factory_t )( );
	extern factory_t get;
}  // namespace photon

#define EXPOSE_PHOTON_MOD( class_t )                                      \
	photon::factory_t   photon::get;                                         \
	DLL_EXPORT class_t* initialize_photon_mod( photon::factory_t factory ) { \
		photon::get = factory;                                                  \
		static class_t mod;                                                     \
		return &mod;                                                            \
	}
