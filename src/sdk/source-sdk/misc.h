#pragma once

#include "vector.h"

enum e_signonstate {
	signonstate_none        = 0,
	signonstate_challenge   = 1,
	signonstate_connected   = 2,
	signonstate_new         = 3,
	signonstate_prespawn    = 4,
	signonstate_spawn       = 5,
	signonstate_full        = 6,
	signonstate_changelevel = 7,
};

typedef unsigned long h_font;

enum e_paint_mode {
	paint_uipanels     = ( 1 << 0 ),
	paint_ingamepanels = ( 1 << 1 ),
};

enum e_font_flags {
	fontflag_none,
	fontflag_italic       = 0x001,
	fontflag_underline    = 0x002,
	fontflag_strikeout    = 0x004,
	fontflag_symbol       = 0x008,
	fontflag_antialias    = 0x010,
	fontflag_gaussianblur = 0x020,
	fontflag_rotary       = 0x040,
	fontflag_dropshadow   = 0x080,
	fontflag_additive     = 0x100,
	fontflag_outline      = 0x200,
	fontflag_custom       = 0x400,
	fontflag_bitmap       = 0x800,
};

enum e_button_code {
	button_code_invalid = -1,
	button_code_none    = 0,

	key_first = 0,

	key_none = key_first,
	key_0,
	key_1,
	key_2,
	key_3,
	key_4,
	key_5,
	key_6,
	key_7,
	key_8,
	key_9,
	key_a,
	key_b,
	key_c,
	key_d,
	key_e,
	key_f,
	key_g,
	key_h,
	key_i,
	key_j,
	key_k,
	key_l,
	key_m,
	key_n,
	key_o,
	key_p,
	key_q,
	key_r,
	key_s,
	key_T,  // linux typedef shit
	key_u,
	key_v,
	key_w,
	key_x,
	key_y,
	key_z,
	key_pad_0,
	key_pad_1,
	key_pad_2,
	key_pad_3,
	key_pad_4,
	key_pad_5,
	key_pad_6,
	key_pad_7,
	key_pad_8,
	key_pad_9,
	key_pad_divide,
	key_pad_multiply,
	key_pad_minus,
	key_pad_plus,
	key_pad_enter,
	key_pad_decimal,
	key_lbracket,
	key_rbracket,
	key_semicolon,
	key_apostrophe,
	key_backquote,
	key_comma,
	key_period,
	key_slash,
	key_backslash,
	key_minus,
	key_equal,
	key_enter,
	key_space,
	key_backspace,
	key_tab,
	key_capslock,
	key_numlock,
	key_escape,
	key_scrolllock,
	key_insert,
	key_delete,
	key_home,
	key_end,
	key_pageup,
	key_pagedown,
	key_break,
	key_lshift,
	key_rshift,
	key_lalt,
	key_ralt,
	key_lcontrol,
	key_rcontrol,
	key_lwin,
	key_rwin,
	key_app,
	key_up,
	key_left,
	key_down,
	key_right,
	key_f1,
	key_f2,
	key_f3,
	key_f4,
	key_f5,
	key_f6,
	key_f7,
	key_f8,
	key_f9,
	key_f10,
	key_f11,
	key_f12,
	key_capslocktoggle,
	key_numlocktoggle,
	key_scrolllocktoggle,

	key_last  = key_scrolllocktoggle,
	key_count = key_last - key_first + 1,

	mouse_first = key_last + 1,

	mouse_left = mouse_first,
	mouse_right,
	mouse_middle,
	mouse_4,
	mouse_5,
	mouse_wheel_up,
	mouse_wheel_down,

	mouse_last  = mouse_wheel_down,
	mouse_count = mouse_last - mouse_first + 1,
};

struct vertex_t {
	vec2_t position;
	vec2_t tex_coord;

	vertex_t( ) {}
	vertex_t( const vec2_t& pos, const vec2_t& coord = { 0, 0 } ) {
		position  = pos;
		tex_coord = coord;
	}
};

enum e_hoststate {
	hs_new_game = 0,
	hs_load_game,
	hs_change_level_sp,
	hs_change_level_mp,
	hs_run,
	hs_game_shutdown,
	hs_shutdown,
	hs_restart,
};

struct c_host_state {
	e_hoststate current_state;
	e_hoststate next_state;
	vec3_t      vec_location;
	vec3_t      ang_location;  // TODO: make qangle class in future ?
	char        level_name[ 256 ];
	char        map_group_name[ 256 ];
	char        landmark_name[ 256 ];
	char        save_name[ 256 ];
	float       short_frame_time;

	bool active_game;
	bool remember_location;
	bool background_level;
	bool waiting_for_connection;
	bool let_tools_override_load_game_ents;
	bool split_screen_connect;
	bool game_has_shut_down_and_flushed_memory;
	bool workshop_map_download_pending;
};

#define FCVAR_NONE                    0
#define FCVAR_UNREGISTERED            ( 1 << 0 )
#define FCVAR_DEVELOPMENTONLY         ( 1 << 1 )
#define FCVAR_GAMEDLL                 ( 1 << 2 )
#define FCVAR_CLIENTDLL               ( 1 << 3 )
#define FCVAR_HIDDEN                  ( 1 << 4 )
#define FCVAR_PROTECTED               ( 1 << 5 )
#define FCVAR_SPONLY                  ( 1 << 6 )
#define FCVAR_ARCHIVE                 ( 1 << 7 )
#define FCVAR_NOTIFY                  ( 1 << 8 )
#define FCVAR_USERINFO                ( 1 << 9 )
#define FCVAR_CHEAT                   ( 1 << 14 )
#define FCVAR_PRINTABLEONLY           ( 1 << 10 )
#define FCVAR_UNLOGGED                ( 1 << 11 )
#define FCVAR_NEVER_AS_STRING         ( 1 << 12 )
#define FCVAR_REPLICATED              ( 1 << 13 )
#define FCVAR_DEMO                    ( 1 << 16 )
#define FCVAR_DONTRECORD              ( 1 << 17 )
#define FCVAR_RELOAD_MATERIALS        ( 1 << 20 )
#define FCVAR_RELOAD_TEXTURES         ( 1 << 21 )
#define FCVAR_NOT_CONNECTED           ( 1 << 22 )
#define FCVAR_MATERIAL_SYSTEM_THREAD  ( 1 << 23 )
#define FCVAR_ARCHIVE_XBOX            ( 1 << 24 )
#define FCVAR_ACCESSIBLE_FROM_THREADS ( 1 << 25 )
#define FCVAR_SERVER_CAN_EXECUTE      ( 1 << 28 )
#define FCVAR_SERVER_CANNOT_QUERY     ( 1 << 29 )
#define FCVAR_CLIENTCMD_CAN_EXECUTE   ( 1 << 30 )
