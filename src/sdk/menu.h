#pragma once

#include "source_sdk.h"

class c_menu {
public:
	virtual bool button( const vec2_t& size, const char* label );                                        // returns true on button click.
	virtual bool toggle( bool& val, const char* label );                                                 // toggle using boolean value.
	virtual void slider( int& val, int min, int max, const char* label );                                // slider using integer values.
	virtual void sliderf( float& val, float min, float max, const char* label );                         // slider using float values.
	virtual void colorpicker( color_t& val, const char* label );                                         // colorpicker using a 4 value datatype (r, g, b, a).
	virtual void combo( size_t& val, const char* items[], size_t items_count, const char* label );       // value is the index of the selected element.
	virtual void multicombo( size_t& val, const char* items[], size_t items_count, const char* label );  // value is a bitmask containing selected elements.
	virtual void textbox( const char*& val, const char* label );                                         // textbox using c-string value.
	virtual void separator( const char* label );                                                         // separator with a label on it.
};
