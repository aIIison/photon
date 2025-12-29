#pragma once

#include <photon.h>

namespace config {
	inline bool        example_boolean{ };
	inline int         example_integer{ };
	inline float       example_float{ };
	inline color_t     example_color{ };
	inline size_t      example_combo_value{ };
	inline size_t      example_mcombo_value{ };
	inline const char* example_textbox_value{ "" };

	bool initialize( );
	void uninitialize( );
};  // namespace config
