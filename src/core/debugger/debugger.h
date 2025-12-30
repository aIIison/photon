#pragma once

#include "util/util.h"

namespace debugger {
	bool initialize( );
	void uninitialize( );
	void disasm( uintptr_t addr );
}  // namespace debugger
