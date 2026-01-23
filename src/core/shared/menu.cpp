#include "sdk/menu.h"

#include "core/menu/framework.h"

#include <array>
#include <iostream>
#include <vector>

bool photon::c_menu_context::button( const vec2_t& size, const char* label ) {
	return gui::framework::button( size, label );
}
bool photon::c_menu_context::toggle( bool& val, const char* label ) {
	return gui::framework::toggle( val, label );
}
void photon::c_menu_context::slider( int& val, int min, int max, const char* label ) {
	return gui::framework::slider( val, min, max, label );
}
void photon::c_menu_context::sliderf( float& val, float min, float max, const char* label ) {
	return gui::framework::sliderf( val, min, max, label );
}
void photon::c_menu_context::colorpicker( color_t& val, const char* label ) {
	return gui::framework::colorpicker( val, label );
}
void photon::c_menu_context::combo( std::size_t& val, const char* items[], std::size_t items_count, const char* label ) {
	std::vector< std::string > items_vector;
	for ( std::size_t i = 0; i < items_count; ++i ) {
		items_vector.push_back( items[ i ] );
	}

	return gui::framework::combo( val, items_vector, label );
}
void photon::c_menu_context::multicombo( std::size_t& val, const char* items[], std::size_t items_count, const char* label ) {
	std::vector< std::string > items_vector;
	for ( std::size_t i = 0; i < items_count; ++i ) {
		items_vector.push_back( items[ i ] );
	}

	return gui::framework::multicombo( val, items_vector, label );
}
void photon::c_menu_context::textbox( const char*& val, const char* label ) {
	return gui::framework::textbox( val, label );
}
void photon::c_menu_context::separator( const char* label ) {
	return gui::framework::separator( label );
}
