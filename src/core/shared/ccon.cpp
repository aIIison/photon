// i wanted to call this "con" but windows wasnt letting me >:(

#include "sdk/ccon.h"

#include "core/interfaces/interfaces.h"
#include "sdk/photon.h"
#include "util/util.h"

static std::unordered_map< std::string, con_command* >          cmds;
static std::unordered_map< std::string, fn_command_callback_t > hooked_cbks;

con_command* photon::c_con::create_cmd( const char* name, fn_command_callback_t cbk, const char* help_string, int flags ) {
	// use the game's memory allocator
	auto cmd = reinterpret_cast< con_command* >( interfaces::mem_alloc->alloc( sizeof( con_command ) ) );
	memset( cmd, 0, sizeof( con_command ) );

	// steal vtable from the game
	cmd->vtable = interfaces::cvar->find_command_base( "listdemo" )->vtable;

	cmd->name                = name;
	cmd->fn_command_callback = cbk;
	cmd->help_string         = help_string;
	cmd->flags               = flags;

	// TODO: implement command completion
	cmd->fn_completion_callback           = 0;
	cmd->has_completion_callback          = false;
	cmd->using_new_command_callback       = true;
	cmd->using_command_callback_interface = false;

	interfaces::cvar->register_con_command( cmd );

	cmds.insert( std::make_pair( name, cmd ) );

	return cmd;
}
void photon::c_con::destruct_cmd( const char* name ) {
	if ( !cmds.count( name ) )
		return;

	auto cmd = cmds[ name ];

	interfaces::cvar->unregister_con_command( cmd );

	interfaces::mem_alloc->free( cmd );

	cmds.erase( name );
}

void photon::c_con::hook_cmd( const char* name, fn_command_callback_t detour ) {
	auto cmd = reinterpret_cast< con_command* >( interfaces::cvar->find_command_base( name ) );

	if ( !cmd )
		return;

	hooked_cbks.insert( std::make_pair( name, cmd->fn_command_callback ) );
	cmd->fn_command_callback = detour;
}
void photon::c_con::unhook_cmd( const char* name ) {
	auto cmd      = reinterpret_cast< con_command* >( interfaces::cvar->find_command_base( name ) );
	auto original = hooked_cbks[ name ];
	hooked_cbks.erase( name );

	if ( !cmd || !original )
		return;

	cmd->fn_command_callback = original;
}

con_var* photon::c_con::find_var( const char* name ) {
	return reinterpret_cast< con_var* >( interfaces::cvar->find_command_base( name ) );
}

con_command* photon::c_con::find_cmd( const char* name ) {
	return reinterpret_cast< con_command* >( interfaces::cvar->find_command_base( name ) );
}
