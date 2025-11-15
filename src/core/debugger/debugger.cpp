#include "debugger.h"

#include <Zydis/Zydis.h>

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
			hex_pos += sprintf_s( hex_buf + hex_pos, sizeof( hex_buf ) - hex_pos,
			                      "%02X ", ( ( unsigned char* ) addr )[ i ] );
		}
		util::console::log( PRINT_CYAN "%p    " PRINT_BLUE "%-40s" PRINT_YELLOW "%s\n", addr, hex_buf, insn.text );

		addr += insn.info.length;

		if ( insn.info.mnemonic == ZYDIS_MNEMONIC_RET )
			break;
	}
}
