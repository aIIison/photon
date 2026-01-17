#pragma once

#include "sdk/photon.h"

#include <unordered_map>

namespace mods {
	struct mod_info_t {
		void*          handle;
		photon::i_mod* ptr;
		bool           is_loaded;  // this is kinda confusing, because the module is loaded at all times, but it shouldnt really do anything until the fn "load" is called, maybe should rename this to is_enabled?
	};

	inline std::unordered_map< std::string, mod_info_t > mod_list;

	bool load( const char* name );
	void unload( const char* name );

	bool loadall( );
	void unloadall( );

	bool enable( mod_info_t* mod );
	void disable( mod_info_t* mod );

	void print( );

	void post_event( const char* msg );
}  // namespace mods
