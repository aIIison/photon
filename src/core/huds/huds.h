#pragma once

#include "sdk/hud.h"

#include <string>
#include <unordered_map>

namespace huds {
	inline std::unordered_map< std::string, photon::i_hud* > huds;

	inline int safezone_x = 8;
	inline int safezone_y = 8;

	inline bool edit = false;

	inline photon::i_hud* cur_hud;

	void draw( );
	void draw_ui( );
}  // namespace huds
