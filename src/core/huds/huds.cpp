#include "huds.h"

#include "core/menu/framework.h"
#include "sdk/photon.h"
#include "util/util.h"

#include <algorithm>
#include <cmath>

struct hud_bounds_t {
	vec2_t mins;    // top left corner
	vec2_t maxs;    // bottom right corner
	vec2_t center;  // center
};

struct point_distance_t {
	vec2_t pt1;   // one of the current hud's points
	vec2_t pt2;   // one of the other hud's points
	float  dist;  // squared distance
};

static vec2_t get_abs_pos( photon_api::i_hud* hud ) {
	const auto pos    = photon->render->to_screen( hud->pos );
	const auto anchor = hud->anchor * hud->bounds;

	return pos - anchor;
}

static void set_abs_pos( photon_api::i_hud* hud, const vec2_t& pos ) {
	const auto screen_size = photon->render->get_screen_size( );

	const auto anchor = hud->anchor * hud->bounds;

	auto new_pos = pos + anchor;

	new_pos.x = std::clamp( new_pos.x, ( float ) huds::safezone_x, screen_size.x - huds::safezone_x );
	new_pos.y = std::clamp( new_pos.y, ( float ) huds::safezone_y, screen_size.y - huds::safezone_y );

	hud->pos = photon->render->normalize( new_pos );
}

static void set_hud_anchor( photon_api::i_hud* hud ) {
	const auto screen_size = photon->render->get_screen_size( );

	const auto center = get_abs_pos( hud ) + hud->bounds / 2;

	if ( ( int ) center.x < screen_size.x / 2 )
		hud->anchor.x = 0.0f;
	else if ( ( int ) center.x > screen_size.x / 2 )
		hud->anchor.x = 1.0f;
	else
		hud->anchor.x = 0.5f;

	if ( ( int ) center.y < screen_size.y / 2 )
		hud->anchor.y = 0.0f;
	else if ( ( int ) center.y > screen_size.y / 2 )
		hud->anchor.y = 1.0f;
	else
		hud->anchor.y = 0.5f;
}

static hud_bounds_t get_hud_bounds( photon_api::i_hud* hud ) {
	const auto hud_pos = get_abs_pos( hud );

	hud_bounds_t bounds;
	bounds.mins   = hud_pos;
	bounds.maxs   = hud_pos + hud->bounds;
	bounds.center = hud_pos + hud->bounds / 2;

	return bounds;
};

/*
 * calculate distances between all edges of two huds
 * pass in a vector to add the distances to
 */
static void calculate_distances( std::vector< point_distance_t >& distances, const hud_bounds_t& hud1, const hud_bounds_t& hud2 ) {
	// loop over all edges
	for ( int i = 0; i < 3; ++i ) {
		for ( int j = 0; j < 3; ++j ) {
			for ( int axis = 0; axis < 2; ++axis ) {
				auto pt1 = ( ( vec2_t* ) &hud1 )[ i ];
				auto pt2 = ( ( vec2_t* ) &hud2 )[ j ];

				// since we align huds on axis, not actually by points, set -1 on the axis we dont use
				pt1[ !axis ] = -1;
				pt2[ !axis ] = -1;

				// delta pos, again only on one axis
				float delta_pos = pt1[ axis ] - pt2[ axis ];

				point_distance_t dist;
				dist.pt1  = pt1;
				dist.pt2  = pt2;
				dist.dist = delta_pos * delta_pos;  // squared distance

				distances.push_back( dist );
			}
		}
	}
}

/*
 * get all distances to all hud element's edges, sort them, align to closest
 * this whole thing might be overcomplicated, but this was my best idea
 */
static void align_hud_element( photon_api::i_hud* hud ) {
	const color_t clr{ 255, 0, 255, 255 };

	const auto screen_size = photon->render->get_screen_size( );
	const auto hud_bounds  = get_hud_bounds( hud );

	std::vector< point_distance_t > distances;

	/*
	 * alignment with screen anchors
	 */
	hud_bounds_t b;
	b.mins   = { huds::safezone_x, huds::safezone_y };
	b.maxs   = { screen_size.x - huds::safezone_x, screen_size.y - huds::safezone_y };
	b.center = screen_size / 2;
	calculate_distances( distances, hud_bounds, b );

	/*
	 * alignment with other hud elements
	 */
	for ( const auto& [ _, other_hud ] : huds::huds ) {
		if ( hud == other_hud )
			continue;

		const auto other_hud_bounds = get_hud_bounds( other_hud );

		calculate_distances( distances, hud_bounds, other_hud_bounds );
	}

	// sort points by distance
	std::sort( distances.begin( ), distances.end( ), []( const point_distance_t& lhs, const point_distance_t& rhs ) {
		return lhs.dist < rhs.dist;
	} );

	// align hud element x, y but only once per axis
	bool was_vertical = false;
	for ( int i = 0; i < 2; ++i ) {
		const auto& pt_dist  = distances[ i ];
		const auto  vertical = pt_dist.pt2.x == -1;  // axis check
		const auto  hud_pos  = get_abs_pos( hud );

		// only align once per axis
		if ( i == 1 && vertical == was_vertical )
			return;

		was_vertical = vertical;

		if ( pt_dist.dist < 100 ) {
			auto pos = photon->render->to_screen( hud->pos );

			if ( !vertical ) {
				pos.x = ( pt_dist.pt2.x - ( pt_dist.pt1.x - hud_pos.x ) );
				pos.x += hud->anchor.x * hud->bounds.x;
				photon->render->draw_line( pt_dist.pt2.x, 0, 0, screen_size.y, clr );
			} else {
				pos.y = ( pt_dist.pt2.y - ( pt_dist.pt1.y - hud_pos.y ) );
				pos.y += hud->anchor.y * hud->bounds.y;
				photon->render->draw_line( 0, pt_dist.pt2.y, screen_size.x, 0, clr );
			}

			hud->pos = photon->render->normalize( pos );
		}
	}
}

void huds::draw( ) {
	for ( const auto& [ _, hud ] : huds ) {
		hud->draw( );

		if ( hud->is_splitscreen( ) && photon->common->is_splitscreen( ) ) {
			const auto  screen_size      = photon->render->get_screen_size( );
			static auto ss_verticalsplit = photon->con->find_var( "ss_verticalsplit" );

			auto axis   = 1 - ss_verticalsplit->get_int( );
			auto offset = photon->render->normalize( screen_size / 2 )[ axis ];

			hud->pos[ axis ] += offset;
			hud->draw( );
			hud->pos[ axis ] -= offset;
		}
	}
}

void huds::draw_ui( ) {
	const color_t clr{ 0, 255, 255, 255 };

	const auto screen_size = photon->render->get_screen_size( );

	static photon_api::i_hud* cur_hud;

	static vec2_t grab_pos;

	// check if clicking on a hud.
	for ( const auto& [ _, hud ] : huds ) {
		const auto pos = get_abs_pos( hud );

		if ( photon->input->is_cursor_in_area( pos.x, pos.y, pos.x + hud->bounds.x, pos.y + hud->bounds.y ) ) {
			photon->render->draw_outlined_rect( pos.x - 1, pos.y - 1, hud->bounds.x + 2, hud->bounds.y + 2, clr );
			photon->render->draw_filled_rect( pos.x + hud->anchor.x * hud->bounds.x - 3, pos.y + hud->anchor.y * hud->bounds.y - 3, 6, 6, clr );

			if ( photon->input->get_key_press( mouse_left ) ) {
				cur_hud  = hud;
				grab_pos = photon->input->get_cursor_position( ) - pos;
			}
		}
	}

	// moving around hud.
	if ( cur_hud ) {
		if ( photon->input->get_key_held( mouse_left ) ) {
			const auto hud = cur_hud;

			set_hud_anchor( hud );

			set_abs_pos( hud, photon->input->get_cursor_position( ) - grab_pos );

			align_hud_element( hud );
		}

		if ( photon->input->get_key_release( mouse_left ) ) {
			cur_hud = nullptr;
		}
	}

	// draw little hint when editing hud layout.
	{
		const vec2_t pos{ screen_size.x - 8, 8 };

		photon->render->draw_text( pos.x, pos.y, gui::framework::fonts::bigtitle, { 255, 255, 255, 64 }, 2, "Currently editing HUD layout." );
		photon->render->draw_text( pos.x, pos.y + 28, gui::framework::fonts::title, { 255, 255, 255, 16 }, 2, "Press ESC to finish editing." );
	}
}
