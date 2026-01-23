#pragma once

#include "sdk/photon.h"
#include "source-sdk/plugin.h"

#include <thread>

#define PLUGIN_SIGNATURE "photon dev"

#define SAFE_UNLOAD_DELAY 33

using namespace photon;

struct plugin_info_t {
	c_plugin* ptr;
	int       index;
};

class c_photon : public i_server_plugin_callbacks {
public:
	plugin_info_t* info;

private:
	bool unloading;

public:
	virtual bool        load( create_interface_fn interface_factory, create_interface_fn game_server_factory );
	virtual void        unload( );
	virtual void        pause( ) {};
	virtual void        un_pause( ) {};
	virtual const char* get_plugin_description( ) { return PLUGIN_SIGNATURE; };
	virtual void        level_init( char const* map_name ) {};
	virtual void        server_activate( void* edict_list, int edict_count, int client_max ) {};
	virtual void        game_frame( bool simulating ) {};
	virtual void        level_shutdown( ) {};
	virtual void        client_fully_connect( void* edict ) {};
	virtual void        client_active( void* entity ) {};
	virtual void        client_disconnect( void* entity ) {};
	virtual void        client_put_in_server( void* entity, char const* playername ) {};
	virtual void        set_command_client( int index ) {};
	virtual void        client_settings_changed( void* edict ) {};
	virtual int         client_connect( bool* allow_connect, void* entity, const char* name, const char* address, char* reject, int maxrejectlen ) { return 0; };
	virtual int         client_command( void* entity, const void*& args ) { return 0; };
	virtual int         network_id_validated( const char* user_name, const char* network_id ) { return 0; };
	virtual void        on_query_cvar_value_finished( int cookie, void* player_entity, int status, const char* cvar_name, const char* cvar_value ) {};
	virtual void        on_edict_allocated( void* edict ) {};
	virtual void        on_edict_freed( const void* edict ) {};

	bool get_info( );
};

extern c_photon plugin;
