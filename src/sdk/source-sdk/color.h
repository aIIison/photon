#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

struct color_t {
	union {
		struct {
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};

		uint32_t rgba;
	};

	color_t( ) {}
	color_t( uint8_t r, uint8_t g, uint8_t b ) : r{ r }, g{ g }, b{ b }, a{ 255 } {}
	color_t( uint8_t r, uint8_t g, uint8_t b, uint8_t a ) : r{ r }, g{ g }, b{ b }, a{ a } {}
	color_t( uint32_t rgba ) : rgba{ rgba } {}

	void to_hsv( float& h, float& s, float& v ) {
		float r = this->r / 255.f;
		float g = this->g / 255.f;
		float b = this->b / 255.f;

		float cmax = std::max( r, std::max( g, b ) );
		float cmin = std::min( r, std::min( g, b ) );
		float diff = cmax - cmin;

		if ( cmax == cmin )
			h = 0;
		else if ( cmax == r )
			h = fmod( ( 60 * ( ( g - b ) / diff ) + 360 ), 360 ) / 360.f;
		else if ( cmax == g )
			h = fmod( ( 60 * ( ( b - r ) / diff ) + 120 ), 360 ) / 360.f;
		else if ( cmax == b )
			h = fmod( ( 60 * ( ( r - g ) / diff ) + 240 ), 360 ) / 360.f;

		if ( cmax == 0 )
			s = 0;
		else
			s = diff / cmax;

		v = cmax;
	}

	static color_t from_hsv( float h, float s, float v ) {
		float r, g, b;

		int   i = floor( h * 6 );
		float f = h * 6 - i;
		float p = v * ( 1 - s );
		float q = v * ( 1 - f * s );
		float t = v * ( 1 - ( 1 - f ) * s );

		switch ( i % 6 ) {
		case 0:
			r = v, g = t, b = p;
			break;
		case 1:
			r = q, g = v, b = p;
			break;
		case 2:
			r = p, g = v, b = t;
			break;
		case 3:
			r = p, g = q, b = v;
			break;
		case 4:
			r = t, g = p, b = v;
			break;
		case 5:
			r = v, g = p, b = q;
			break;
		}

		return {
			( uint8_t ) ( r * 255 ),
			( uint8_t ) ( g * 255 ),
			( uint8_t ) ( b * 255 )
		};
	}
};
