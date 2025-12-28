#include "debugger.h"

#include "core/photon.h"

#include <Zydis/Zydis.h>

util::console::cmd_t clear( "clear", "clear - clear console.", []( util::console::cmd_t::args_t args ) -> bool {
	util::console::clear( );
	return true;
} );

util::console::cmd_t d( "d", "d <addr> - disassemble code at address.", []( util::console::cmd_t::args_t args ) -> bool {
	if ( args.size( ) > 1 ) {
		uintptr_t addr;
		sscanf( args[ 1 ].c_str( ), "%p", &addr );
		debugger::disasm( addr );
		return true;
	} else {
		return false;
	}
} );

util::console::cmd_t ss( "ss", "ss <module> <signature> - signature scan.", []( util::console::cmd_t::args_t args ) -> bool {
	if ( args.size( ) > 2 ) {
		std::string pattern;
		for ( size_t i = 2; i < args.size( ); ++i ) {
			pattern += args[ i ];
			if ( i != args.size( ) - 1 )
				pattern += " ";
		}
		util::pattern_scan( args[ 1 ].c_str( ), pattern.c_str( ) );
		return true;
	} else {
		return false;
	}
} );

// e_return_action fprintf_nop( e_callback_type type, signal_context_t* ctx ) {
// 	return e_return_action::Supercede;
// }

void debugger::initialize( ) {
	util::console::alloc( );

#ifdef _WIN32
	SetConsoleTitleA( "photon debug" );
#endif

	util::console::log( PRINT_GREEN "[+] photon debug > type `help` for a list of commands.\n" );
}

void debugger::uninitialize( ) {
	util::console::free( );
}

void debugger::disasm( uintptr_t addr ) {
	ZydisDisassembledInstruction insn;
	while ( ZYAN_SUCCESS( ZydisDisassembleIntel(
					/* machine_mode:    */ ZYDIS_MACHINE_MODE_LEGACY_32,
					/* runtime_address: */ addr,
					/* buffer:          */ ( void* ) addr,
					/* length:          */ 1024,
					/* instruction:     */ &insn ) ) ) {

		if ( insn.info.opcode == 0xCC )
			break;

		char hex_buf[ 64 ] = { 0 };
		int  hex_pos       = 0;
		for ( ZyanU8 i = 0; i < insn.info.length; i++ ) {
			hex_pos += snprintf( hex_buf + hex_pos, sizeof( hex_buf ) - hex_pos,
			                     "%02X ", ( ( unsigned char* ) addr )[ i ] );
		}
		util::console::log( PRINT_CYAN "%p    " PRINT_BLUE "%-40s" PRINT_YELLOW "%s\n", addr, hex_buf, insn.text );

		addr += insn.info.length;

		if ( insn.info.mnemonic == ZYDIS_MNEMONIC_RET )
			break;
	}
}
