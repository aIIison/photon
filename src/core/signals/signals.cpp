#include "signals.h"

#include "core/huds/huds.h"
#include "core/interfaces/interfaces.h"
#include "core/menu/gui.h"
#include "core/photon.h"

// e_return_action game_frame( bool, simulating ) {
// 	photon->common->post_event( &plugin, "pre_tick" );

// 	original( ecx, simulating );

// 	photon->common->post_event( &plugin, "post_tick" );
// }

// e_return_action frame( ) {
// 	photon->common->post_event( &plugin, "pre_frame" );

// 	original( ecx );

// 	// XXX: im not sure why we need this condition here, it used to be fine
// 	// possibly i changed something about unloading and this doesnt get unhooked early enough?
// 	// if there is more stuff being done here later, this might break, this should be looked into !!!
// 	if ( photon )
// 		photon->common->post_event( &plugin, "post_frame" );
// }

e_return_action set_signon_state( e_callback_type type, signal_params_t* params ) {
	auto state = params->get_arg< int >( 1 );

	// this is probably not the best way, i saw SAR do something similar but this needs further thought
	if ( state == signonstate_full )
		photon->common->post_event( &plugin, "session_start" );
	else
		photon->common->post_event( &plugin, "session_end" );

	return e_return_action::Handled;
}

e_return_action on_screen_size_changed( e_callback_type type, signal_params_t* params ) {
	photon->common->post_event( &plugin, "on_screen_size_changed" );

	// recreate fonts
	gui::create_fonts( );

	return e_return_action::Handled;
}

bool signals::initialize( ) {
	// photon->signal->create( "CServerGameDLL::GameFrame" )->with_parameters( { Pointer, Bool } )->in_module( MODULE( "server" ) )->in_interface( "ServerGameDLL005" )->from_vtable( 4 )->enable( );
	// photon->signal->create( "CEngine::Frame" )->with_parameters( { Pointer } )->at_address( interfaces::engine )->from_vtable( OS( 5, 6 ) )->enable( );
	photon->signal->create( "CClientState::SetSignonState" )->with_parameters( { Pointer, Int32, Int32, Int32 } )->at_address( interfaces::client_state )->from_vtable( OS( 15, 36 ) )->enable( );
	photon->signal->create( "CEngineVGui::Paint" )->with_parameters( { Pointer, Int32 } )->in_module( MODULE( "engine" ) )->in_interface( "VEngineVGui001" )->from_vtable( OS( 14, 15 ) )->enable( );
	photon->signal->create( "CMatSystemSurface::LockCursor" )->with_parameters( { Pointer } )->at_address( interfaces::surface )->from_vtable( 65 )->enable( );
	photon->signal->create( "CHLClient::IN_KeyEvent" )->with_parameters( { Pointer, Int32, Int32, String } )->in_module( MODULE( "client" ) )->in_interface( "VClient016" )->from_vtable( 20 )->enable( );
	photon->signal->create( "CEngineVGui::UpdateButtonState" )->with_parameters( { Pointer, Pointer } )->in_module( MODULE( "vgui2" ) )->in_interface( "VGUI_Input005" )->from_vtable( OS( 87, 88 ) )->enable( );
	photon->signal->create( "CMatSystemSurface::OnScreenSizeChanged" )->with_parameters( { Pointer, Int32, Int32 } )->at_address( interfaces::surface )->from_vtable( 114 )->enable( );

	// photon->signal->get( "CServerGameDLL::GameFrame" )->add_callback( &game_frame );
	// photon->signal->get( "CEngine::Frame" )->add_callback( &frame );
	photon->signal->get( "CClientState::SetSignonState" )->add_callback( e_callback_type::Post, &set_signon_state );
	photon->signal->get( "CMatSystemSurface::OnScreenSizeChanged" )->add_callback( e_callback_type::Post, &on_screen_size_changed );

	return true;
}

void signals::uninitialize( ) {
	photon->signal->remove_all( );
}
