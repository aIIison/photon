#include "convars.h"

#include "core/interfaces/interfaces.h"
#include "core/mods/mods.h"
#include "sdk/photon.h"

static con_var* ui_loadingscreen_transition_time;
static con_var* ui_loadingscreen_fadein_time;
static con_var* ui_loadingscreen_mintransition_time;

void convars::set_fast_loads( bool enable ) {
	// TODO: do the stuff that SAR does to make loads even faster
	ui_loadingscreen_transition_time->set_value( !enable );
	ui_loadingscreen_fadein_time->set_value( !enable );
	ui_loadingscreen_mintransition_time->set_value( enable ? 0.0f : 0.5f );
}

static void photon_load( const c_command& args ) {
	if ( args.arg_c( ) >= 2 ) {
		mods::load( args[ 1 ] );
	} else {
		photon->common->log( "photon_load <mod name> - load a photon mod.\n" );
	}
}

static void photon_unload( const c_command& args ) {
	if ( args.arg_c( ) >= 2 ) {
		mods::unload( args[ 1 ] );
	} else {
		photon->common->log( "photon_unload <mod name> - unload a photon mod.\n" );
	}
}

static void photon_enable( const c_command& args ) {
	if ( args.arg_c( ) >= 2 ) {
		mods::enable( &mods::mod_list[ args[ 1 ] ] );
	} else {
		photon->common->log( "photon_enable <mod name> - enable a photon mod.\n" );
	}
}

static void photon_disable( const c_command& args ) {
	if ( args.arg_c( ) >= 2 ) {
		mods::disable( &mods::mod_list[ args[ 1 ] ] );
	} else {
		photon->common->log( "photon_disable <mod name> - disable a photon mod.\n" );
	}
}

static void photon_print( const c_command& args ) {
	mods::print( );
}

static void photon_debug( const c_command& args ) {
	if ( util::console::alloc( ) )
		photon->common->log( { 0, 255, 0 }, "enabled debug mode.\n" );
	else
		photon->common->log_warn( "already in debug mode.\n" );
}

bool convars::initialize( ) {
	photon->con->create_concmd( "photon_load", photon_load, "photon_load <mod name> - load a photon mod.\n" );
	photon->con->create_concmd( "photon_unload", photon_unload, "photon_unload <mod name> - unload a photon mod.\n" );
	photon->con->create_concmd( "photon_enable", photon_enable, "photon_enable <mod name> - enable a photon mod.\n" );
	photon->con->create_concmd( "photon_disable", photon_disable, "photon_disable <mod name> - disable a photon mod.\n" );
	photon->con->create_concmd( "photon_print", photon_print, "prints a list of all loaded photon mods.\n" );
	photon->con->create_concmd( "photon_debug", photon_debug, "enables debug mode.\n" );

	// fast loads
	ui_loadingscreen_transition_time    = photon->con->find_con_var( "ui_loadingscreen_transition_time" );
	ui_loadingscreen_fadein_time        = photon->con->find_con_var( "ui_loadingscreen_fadein_time" );
	ui_loadingscreen_mintransition_time = photon->con->find_con_var( "ui_loadingscreen_mintransition_time" );

	ui_loadingscreen_transition_time->remove_flag( FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN );
	ui_loadingscreen_fadein_time->remove_flag( FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN );
	ui_loadingscreen_mintransition_time->remove_flag( FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN );

	set_fast_loads( true );

	return true;
}

void convars::uninitialize( ) {
	set_fast_loads( false );

	ui_loadingscreen_transition_time->add_flag( FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN );
	ui_loadingscreen_fadein_time->add_flag( FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN );
	ui_loadingscreen_mintransition_time->add_flag( FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN );

	photon->con->destruct_concmd( "photon_debug" );
	photon->con->destruct_concmd( "photon_print" );
	photon->con->destruct_concmd( "photon_disable" );
	photon->con->destruct_concmd( "photon_enable" );
	photon->con->destruct_concmd( "photon_unload" );
	photon->con->destruct_concmd( "photon_load" );
}
