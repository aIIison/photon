#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
// clang-format off
#include <windows.h>
#include <memoryapi.h>
// clang-format on
#else
#include <dlfcn.h>
#include <sys/mman.h>
#define MAX_PATH 4096
#endif

#ifndef _WIN32
#define _stricmp strcasecmp
#endif

#include "address.h"
#include "math.h"
#include "sdk/platform.h"

#define PRINT_BLACK   "\x1b[90m"
#define PRINT_RED     "\x1b[91m"
#define PRINT_GREEN   "\x1b[92m"
#define PRINT_YELLOW  "\x1b[93m"
#define PRINT_BLUE    "\x1b[94m"
#define PRINT_MAGENTA "\x1b[95m"
#define PRINT_CYAN    "\x1b[96m"
#define PRINT_WHITE   "\x1b[97m"
#define PRINT_RESET   "\x1b[0m"

namespace util {
	struct module_info_t {
		char           name[ MAX_PATH ];
		char           path[ MAX_PATH ];
		std::uintptr_t addr;
		std::size_t    size;
	};

	bool      get_module_info( const char* module_name, module_info_t* module_info );
	address_t pattern_scan( const char* module_name, const char* signature ) noexcept;
	address_t get_interface( const char* module_name, const char* interface_name );

	template < std::size_t index, typename t, typename... args_t >
	__forceinline t call_virtual( void* name, args_t... args ) {
		using fn_t = t( __thiscall* )( void*, args_t... );

		auto fn = ( *reinterpret_cast< fn_t** >( name ) )[ index ];
		return fn( name, args... );
	}

	template < std::size_t index >
	__forceinline address_t get_virtual( void* name ) {
		return ( *static_cast< uintptr_t** >( name ) )[ index ];
	}

	template < typename t >
	__forceinline t get_sym_addr( void* module_handle, const char* sym ) {
#ifdef _WIN32
		return ( t ) GetProcAddress( ( HMODULE ) module_handle, sym );
#else
		return ( t ) dlsym( module_handle, sym );
#endif
	}

	__forceinline void* get_module_handle( const char* module_name ) {
		auto info = module_info_t( );
		if ( !get_module_info( module_name, &info ) )
			return nullptr;
#ifdef _WIN32
		return GetModuleHandleA( info.path );
#else
		return dlopen( info.path, RTLD_NOLOAD | RTLD_NOW );
#endif
	}

	std::string                ssprintf( const char* fmt, ... );
	bool                       replace( std::string& str, const std::string& from, const std::string& to );
	std::vector< std::string > split( const std::string& str );

	namespace console {
		using view_t = std::vector< std::string >;
		inline std::vector< view_t* > views{ new view_t{} };
		inline int                    cur_view{ };

		class cmd_t {
		public:
			using args_t   = const std::vector< std::string >&;
			using cmd_fn_t = std::function< bool( args_t ) >;

			const char* helpstr;
			cmd_fn_t    fn;

			cmd_t( const char* name, const char* helpstr, cmd_fn_t fn );
		};

		bool alloc( );
		void free( );
		void handler( );
		void render( bool force_redraw = false );

		template < typename... args_t >
		void add_to_view( int view_idx, const char* fmt, const args_t&... args ) {
			auto& view = views[ view_idx ];
			if ( view ) {
				view->push_back( ssprintf( ( std::string( fmt ) + PRINT_RESET ).c_str( ), args... ) );
				// limit buffer size.
				if ( view->size( ) > 1000 )
					view->erase( view->begin( ) );

				if ( cur_view == view_idx )
					render( );
			}
		}

		template < typename... args_t >
		void log( const char* fmt, const args_t&... args ) {
			add_to_view( 0, ( std::string( fmt ) + PRINT_RESET ).c_str( ), args... );
		}

		template < typename... args_t >
		void log_error( const char* fmt, const args_t&... args ) {
			add_to_view( 0, ( PRINT_RED + std::string( fmt ) + PRINT_RESET ).c_str( ), args... );
		}

		template < typename... args_t >
		void print( const char* fmt, const args_t&... args ) {
			add_to_view( cur_view, ( std::string( fmt ) + PRINT_RESET ).c_str( ), args... );
		}

		__forceinline void clear( ) {
#ifdef _WIN32
			system( "cls" );
#else
			system( "clear" );
#endif
		}
	}  // namespace console
}  // namespace util
