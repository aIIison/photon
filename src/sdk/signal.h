#pragma once

#include "platform.h"

#include <vector>

namespace photon {
	enum e_data_type : uint8_t {
		Void,
		Bool,
		Int8,
		Uint8,
		Int16,
		Uint16,
		Int32,
		Uint32,
		Int64,
		Uint64,
		Float,
		Double,
		Pointer,
		String,
		Wstring,
		M128,
		M256,
		M512,
		Object
	};

	enum e_callconv : uint8_t {
		Cdecl,
		Stdcall,
		Thiscall
	};

	enum class e_callback_type : bool {
		Pre,
		Post
	};

	enum class e_return_action : int32_t {
		Ignored,   // handler didn't take any action
		Handled,   // we did something, but real function should still be called
		Override,  // call real function, but use my return value
		Supercede  // skip real function; use my return value
	};

	union u_data {
		void*    p;
		char*    psz;
		bool     b;
		int32_t  i32;
		uint32_t u32;
		float    f32;
	};

	struct signal_context_t;

	class c_signal_builder {
		friend class c_signal;

	private:
		void* signal;  // opaque

	public:
		virtual c_signal_builder* with_return_type( e_data_type type );
		virtual c_signal_builder* with_callconv( e_callconv callconv );
		virtual c_signal_builder* with_parameters( const std::vector< e_data_type >& params );

		virtual c_signal_builder* in_module( const char* name );
		virtual c_signal_builder* in_interface( const char* name );
		virtual c_signal_builder* at_address( void* address );
		virtual c_signal_builder* from_vtable( size_t index );
		virtual c_signal_builder* from_pattern( const char* pattern );

		virtual c_signal_builder* add_callback( e_callback_type type, void* fn );
		virtual c_signal_builder* remove_callback( e_callback_type type, void* fn );

		virtual c_signal_builder* enable( );
		virtual c_signal_builder* disable( );
	};

	class c_signal {
	public:
		virtual c_signal_builder* create( const char* name );
		virtual void              remove( const char* name );
		virtual void              enable( c_signal_builder* signal );
		virtual void              disable( c_signal_builder* signal );
		virtual c_signal_builder* get( const char* name );
		virtual void              remove_all( );
		virtual u_data            get_arg( signal_context_t* ctx, size_t index );
		virtual void              set_arg( signal_context_t* ctx, size_t index, u_data value );
		virtual u_data            get_return( signal_context_t* ctx );
		virtual void              set_return( signal_context_t* ctx, u_data value );
	};
};  // namespace photon
