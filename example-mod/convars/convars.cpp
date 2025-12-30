#include "convars.h"

#include <photon.h>

static void example_command( const c_command& args ) {
	photon->common->log( "hey!\n" );
}

bool convars::initialize( ) {
	photon->con->create_cmd( "example_command", example_command, "prints \"hey!\" to the console.\n" );

	return true;
}

void convars::uninitialize( ) {
	photon->con->destruct_cmd( "example_command" );
}
