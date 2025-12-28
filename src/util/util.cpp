#include "util.h"

#include "core/debugger/debugger.h"
#include "memory.h"
#include "string.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifdef _WIN32
#include <psapi.h>
#else
#include <link.h>
#endif

static std::atomic< bool > console_running{ false };
static std::thread         console_thread;

static inline auto& console_cmds( ) {
	static std::unordered_map< std::string, util::console::cmd_t* > cmds;
	return cmds;
}

util::console::cmd_t::cmd_t( const char* name, const char* helpstr, cmd_fn_t fn ) {
	this->helpstr = helpstr;
	this->fn      = fn;

	console_cmds( ).insert( { name, this } );
}

util::console::cmd_t help( "help", "help - list off all console commands.", []( util::console::cmd_t::args_t args ) -> bool {
	util::console::log( "[>] list of commands:\n" );
	for ( auto& [ _, cmd ] : console_cmds( ) ) {
		util::console::log( "        %s\n", cmd->helpstr );
	}
	return true;
} );

bool util::console::alloc( ) {
#ifdef _WIN32
	if ( GetConsoleWindow( ) )
		return false;

	AllocConsole( );

	FILE* file;
	freopen_s( &file, "CONIN$", "r", stdin );
	freopen_s( &file, "CONOUT$", "w", stdout );
	freopen_s( &file, "CONOUT$", "w", stderr );

	std::cin.clear( );
	std::cout.clear( );
	std::cerr.clear( );

	// enable colors
	HANDLE out = GetStdHandle( STD_OUTPUT_HANDLE );
	HANDLE in  = GetStdHandle( STD_INPUT_HANDLE );

	DWORD mode = 0;
	GetConsoleMode( out, &mode );
	mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT;
	SetConsoleMode( out, mode );

	GetConsoleMode( in, &mode );
	mode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
	SetConsoleMode( in, mode );
#endif

	console_running = true;
	console_thread  = std::thread( handler );

	return true;
}

void util::console::free( ) {
	if ( console_running ) {
		console_running = false;

		if ( console_thread.joinable( ) )
			console_thread.detach( );
	}

#ifdef _WIN32
	FreeConsole( );
#endif
}

void util::console::handler( ) {
	std::string input;
	while ( console_running ) {
		std::getline( std::cin, input );

		if ( !console_running )
			break;

		if ( !input.empty( ) ) {
			const auto args = split( input );
			auto&      arg0 = args[ 0 ];

			if ( console_cmds( ).contains( arg0 ) ) {
				auto& cmd = console_cmds( )[ arg0 ];

				if ( !cmd->fn( args ) ) {
					util::console::log( PRINT_RED "[!] wrong amount of arguments for command `%s`.\n", arg0.c_str( ) );
					util::console::log( PRINT_RED "[!] %s\n", cmd->helpstr );
				}
			} else {
				util::console::log( PRINT_RED "[!] unknown command `%s`, type `help` for a list of commands.\n", arg0.c_str( ) );
			}
		}
	}
}

static std::vector< util::module_info_t > g_module_info;

bool util::get_module_info( const char* module_name, module_info_t* module_info ) {
	if ( g_module_info.empty( ) ) {
#ifdef _WIN32
		HMODULE mods[ 1024 ];
		HANDLE  proc = GetCurrentProcess( );
		DWORD   cb_needed;

		if ( EnumProcessModules( proc, mods, sizeof( mods ), &cb_needed ) ) {
			for ( unsigned int i = 0; i < ( cb_needed / sizeof( HMODULE ) ); ++i ) {
				char sz_mod_name[ MAX_PATH ];

				if ( GetModuleFileNameA( mods[ i ], sz_mod_name, sizeof( sz_mod_name ) ) ) {
					auto info = MODULEINFO( );

					if ( !GetModuleInformation( proc, mods[ i ], &info, sizeof( info ) ) )
						continue;

					auto name  = std::string( sz_mod_name );
					auto index = name.find_last_of( "\\/" );
					name       = name.substr( index + 1, name.length( ) - index );

					module_info_t mod_info;

					std::snprintf( mod_info.name, sizeof( mod_info.name ), "%s", name.c_str( ) );
					std::snprintf( mod_info.path, sizeof( mod_info.path ), "%s", sz_mod_name );
					mod_info.addr = ( std::uintptr_t ) info.lpBaseOfDll;
					mod_info.size = ( std::size_t ) info.SizeOfImage;

					g_module_info.push_back( mod_info );
				}
			}
		}
#else
		dl_iterate_phdr( []( struct dl_phdr_info* info, std::size_t, void* ) {
			module_info_t mod_info;

			auto name  = std::string( info->dlpi_name );
			auto index = name.find_last_of( "\\/" );
			name       = name.substr( index + 1, name.length( ) - index );

			std::strncpy( mod_info.name, name.c_str( ), sizeof( mod_info.name ) );
			std::strncpy( mod_info.path, info->dlpi_name, sizeof( mod_info.path ) );
			mod_info.addr = info->dlpi_addr + info->dlpi_phdr[ 0 ].p_vaddr;
			mod_info.size = info->dlpi_phdr[ 0 ].p_memsz;

			g_module_info.push_back( mod_info );

			return 0;
		},
		                 nullptr );
#endif
	}

	for ( const auto& info : g_module_info ) {
		if ( _stricmp( info.name, module_name ) )
			continue;

		if ( module_info != nullptr ) {
			*module_info = info;
		}
		return true;
	}

	return false;
}

address_t util::get_interface( const char* module_name, const char* interface_name ) {
	using create_interface_fn = void* ( * ) ( const char*, int* );

	const auto fn = get_sym_addr< create_interface_fn >( get_module_handle( module_name ), "CreateInterface" );

	if ( fn ) {
		void* result = nullptr;

		result = fn( interface_name, nullptr );

		if ( !result ) {
			console::log_error( "[!] couldn't find interface %s in %s.\n", interface_name, module_name );
			return nullptr;
		}

		console::log( "[+] found interface " PRINT_YELLOW "%s" PRINT_RESET " in " PRINT_CYAN "%s [%p]" PRINT_RESET ".\n", interface_name, module_name, result );

		return result;
	}

	console::log_error( "[!] couldn't find CreateInterface fn in %s.\n", module_name );

	return nullptr;
}

address_t util::pattern_scan( const char* module_name, const char* signature ) noexcept {
	module_info_t module_info;

	if ( !get_module_info( module_name, &module_info ) )
		return nullptr;

	static auto pattern_to_byte = []( const char* pattern ) {
		auto bytes = std::vector< int >{ };
		auto start = const_cast< char* >( pattern );
		auto end   = const_cast< char* >( pattern ) + std::strlen( pattern );

		for ( auto current = start; current < end; ++current ) {
			if ( *current == '?' ) {
				++current;

				if ( *current == '?' )
					++current;

				bytes.push_back( -1 );
			} else {
				bytes.push_back( std::strtoul( current, &current, 16 ) );
			}
		}
		return bytes;
	};

	auto size_of_image = module_info.size;

	auto pattern_bytes = pattern_to_byte( signature );
	auto scan_bytes    = reinterpret_cast< std::uint8_t* >( module_info.addr );

	auto s = pattern_bytes.size( );
	auto d = pattern_bytes.data( );

	for ( auto i = 0ul; i < size_of_image - s; ++i ) {
		bool found = true;

		for ( auto j = 0ul; j < s; ++j ) {
			if ( scan_bytes[ i + j ] != d[ j ] && d[ j ] != -1 ) {
				found = false;
				break;
			}
		}
		if ( found ) {
			console::log( "[+] found signature " PRINT_YELLOW "%s" PRINT_RESET " in " PRINT_CYAN "%s [%p]" PRINT_RESET ".\n", signature, module_name, &scan_bytes[ i ] );
			return &scan_bytes[ i ];
		}
	}

	console::log( PRINT_RED "[!] couldn't find signature %s in %s.\n", signature, module_name );
	return nullptr;
}

std::string util::ssprintf( const char* fmt, ... ) {
	va_list ap1, ap2;
	va_start( ap1, fmt );
	va_copy( ap2, ap1 );
	size_t sz = vsnprintf( NULL, 0, fmt, ap1 ) + 1;
	va_end( ap1 );
	char* buf = ( char* ) malloc( sz );
	vsnprintf( buf, sz, fmt, ap2 );
	va_end( ap2 );
	std::string str( buf );
	free( buf );
	return str;
}

bool util::replace( std::string& str, const std::string& from, const std::string& to ) {
	size_t start_pos = str.find( from );
	if ( start_pos == std::string::npos )
		return false;
	str.replace( start_pos, from.length( ), to );
	return true;
}

std::vector< std::string > util::split( const std::string& str ) {
	std::stringstream                    ss( str );
	std::istream_iterator< std::string > begin( ss );
	std::istream_iterator< std::string > end;
	std::vector< std::string >           vec( begin, end );
	return vec;
}
