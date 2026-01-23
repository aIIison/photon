#pragma once

#include "source_sdk.h"

namespace photon {
	class c_config {
	public:
		virtual void begin( const char* module_name );  // begin module serialization.
		virtual void end( );                            // end module serialization.

		virtual void serialize( const char* var_name, const char*& var );
		virtual void serialize( const char* var_name, bool& var );
		virtual void serialize( const char* var_name, int32_t& var );
		virtual void serialize( const char* var_name, uint32_t& var );
		virtual void serialize( const char* var_name, float& var );
		virtual void serialize( const char* var_name, color_t& var );

		virtual void deserialize( const char* module_name );  // deserialize all variables from module.
	};
};  // namespace photon
