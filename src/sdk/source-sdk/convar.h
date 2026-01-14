#pragma once

#include "../platform.h"
#include "color.h"
#include "utlvector.h"

class c_command {
public:
	int         arg_c( ) const { return argc; };
	const char* operator[]( int index ) const { return arg( index ); };
	const char* arg( int index ) const { return argv[ index ]; };

	enum {
		command_max_argc   = 64,
		command_max_length = 512,
	};
	int         argc;
	int         argv0_size;
	char        arg_s_buffer[ command_max_length ];
	char        argv_buffer[ command_max_length ];
	const char* argv[ command_max_argc ];
};

typedef void ( *fn_command_callback_t )( const c_command& command );
typedef int ( *fn_command_completion_callback_t )( const char* partial, char commands[ 64 ][ 64 ] );

class con_command_base {
public:
	void*             vtable;
	con_command_base* next;
	bool              registered;
	const char*       name;
	const char*       help_string;
	int               flags;

	void add_flag( int flag ) {
		flags |= flag;
	}
	void remove_flag( int flag ) {
		flags &= ~flag;
	}
};

class con_command : public con_command_base {
public:
	union {
		void*                 fn_command_callback_v1;
		fn_command_callback_t fn_command_callback;
		void*                 command_callback;
	};
	union {
		fn_command_completion_callback_t fn_completion_callback;
		void*                            command_completion_callback;
	};
	bool has_completion_callback          : 1;
	bool using_new_command_callback       : 1;
	bool using_command_callback_interface : 1;
};

class i_con_var {
public:
	virtual void        set_value( const char* value )        = 0;
	virtual void        set_value( float value )              = 0;
	virtual void        set_value( int value )                = 0;
	virtual void        set_value( color_t value )            = 0;
	virtual const char* get_name( void ) const                = 0;
	virtual const char* get_base_name( void ) const           = 0;
	virtual bool        is_flag_set( int flag ) const         = 0;
	virtual int         get_split_screen_player_slot( ) const = 0;
};

typedef void ( *fn_change_callback_t )( i_con_var* var, const char* p_old_value, float fl_old_value );

class con_var : public con_command_base {
public:
	void*                              vtable_convar;
	con_var*                           parent;
	const char*                        default_value;
	char*                              string;
	int                                string_length;
	float                              f_value;
	int                                n_value;
	bool                               has_min;
	float                              min_val;
	bool                               has_max;
	float                              max_val;
	utl_vector< fn_change_callback_t > fn_change_callback;

	float get_float( ) const {
		return f_value;
	}
	float get_int( ) const {
		return n_value;
	}
	__forceinline color_t get_color( ) const {
		uint8_t* color = ( uint8_t* ) &n_value;
		return { color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] };
	}
	__forceinline bool get_bool( ) const {
		return !!get_int( );
	}
	__forceinline const char* get_string( ) const {
		return string;
	}

	void set_value( const char* value ) {
		using original_fn = void( __thiscall* )( void*, const char* );
		return ( *( original_fn** ) this )[ OS( 12, 19 ) ]( this, value );
	}
	void set_value( float value ) {
		using original_fn = void( __thiscall* )( void*, float );
		return ( *( original_fn** ) this )[ OS( 13, 20 ) ]( this, value );
	}
	void set_value( int value ) {
		using original_fn = void( __thiscall* )( void*, int );
		return ( *( original_fn** ) this )[ OS( 14, 21 ) ]( this, value );
	}
};
