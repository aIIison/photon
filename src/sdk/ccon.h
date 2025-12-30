// i wanted to call this "con" but windows wasnt letting me >:(

#pragma once

#include "source_sdk.h"

#include <unordered_map>

class c_con {
public:
	// todo: add completion func
	virtual con_command* create_cmd( const char* name, fn_command_callback_t cbk, const char* help_string = 0, int flags = 0 );  // create concommands.
	virtual void         destruct_cmd( const char* name );                                                                       // destruct concommands.

	virtual void hook_cmd( const char* name, fn_command_callback_t detour );  // replace concommand callback to your own.
	virtual void unhook_cmd( const char* name );                              // revert concommand callback.

	virtual con_var*     find_var( const char* name );  // find a convar's pointer by name.
	virtual con_command* find_cmd( const char* name );  // find a concommand's pointer bu name.
};
