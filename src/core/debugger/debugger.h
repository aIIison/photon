#pragma once

#include "util/util.h"

namespace debugger {
	void initialize( );
	void uninitialize( );
	void disasm( uintptr_t addr );
}  // namespace debugger
