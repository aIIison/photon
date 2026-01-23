#pragma once

#include "source_sdk.h"

namespace photon {
	class c_common {
	public:
		virtual void log( const char* fmt, ... );
		virtual void log( const color_t& clr, const char* fmt, ... );
		virtual void log_warn( const char* fmt, ... );
		virtual void post_event( const char* msg );

		virtual void* get_interface( const char* module_name, const char* interface_name );
		virtual void* pattern_scan( const char* module_name, const char* signature );
		virtual void* get_module_handle( const char* module_name );

		virtual int           get_slot( );
		virtual bool          is_coop( );
		virtual bool          is_orange( );
		virtual bool          is_splitscreen( );
		virtual const char*   get_current_map( );
		virtual c_host_state* get_host_state( );
		virtual void*         get_client_entity( int entnum );
	};
};  // namespace photon
