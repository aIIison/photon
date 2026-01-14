#pragma once

#include <cmath>

class vec2_t {
public:
	float x, y;

	vec2_t( ) {}
	vec2_t( float x, float y ) : x{ x }, y{ y } {}

	// int for screen-space stuff.
	vec2_t( int x, int y ) : x{ ( float ) x }, y{ ( float ) y } {}
	vec2_t( float x, int y ) : x{ x }, y{ ( float ) y } {}
	vec2_t( int x, float y ) : x{ ( float ) x }, y{ y } {}

	inline float& operator[]( int i ) {
		return ( ( float* ) this )[ i ];
	}
	inline float operator[]( int i ) const {
		return ( ( float* ) this )[ i ];
	}
	inline vec2_t operator+( const vec2_t& v ) const {
		return { x + v.x, y + v.y };
	}
	inline vec2_t operator-( const vec2_t& v ) const {
		return { x - v.x, y - v.y };
	}
	inline vec2_t operator*( const vec2_t& v ) const {
		return { x * v.x, y * v.y };
	}
	inline vec2_t operator/( const vec2_t& v ) const {
		return { x / v.x, y / v.y };
	}
	inline vec2_t operator*( const float& fl ) const {
		return { x * fl, y * fl };
	}
	inline vec2_t operator/( const float& fl ) const {
		return { x / fl, y / fl };
	}
	inline vec2_t& operator+=( const vec2_t& v ) {
		x += v.x;
		y += v.y;
		return *this;
	}
	inline vec2_t& operator-=( const vec2_t& v ) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
	inline vec2_t& operator*=( const vec2_t& v ) {
		x *= v.x;
		y *= v.y;
		return *this;
	}
	inline vec2_t& operator/=( const vec2_t& v ) {
		x /= v.x;
		y /= v.y;
		return *this;
	}
	inline vec2_t& operator*=( const float& fl ) {
		x *= fl;
		y *= fl;
		return *this;
	}
	inline vec2_t& operator/=( const float& fl ) {
		x /= fl;
		y /= fl;
		return *this;
	}

	inline float length_sqr( ) const {
		return ( x * x ) + ( y * y );
	}
	inline float length( ) const {
		return std::sqrt( length_sqr( ) );
	}
};

class vec3_t {
public:
	float x, y, z;

	vec3_t( ) {}
	vec3_t( float x, float y, float z ) : x{ x }, y{ y }, z{ z } {}

	inline float& operator[]( int i ) {
		return ( ( float* ) this )[ i ];
	}
	inline float operator[]( int i ) const {
		return ( ( float* ) this )[ i ];
	}
	inline vec3_t operator+( const vec3_t& v ) const {
		return { x + v.x, y + v.y, z + v.z };
	}
	inline vec3_t operator-( const vec3_t& v ) const {
		return { x - v.x, y - v.y, z - v.z };
	}
	inline vec3_t operator*( const vec3_t& v ) const {
		return { x * v.x, y * v.y, z * v.z };
	}
	inline vec3_t operator/( const vec3_t& v ) const {
		return { x / v.x, y / v.y, z / v.z };
	}
	inline vec3_t operator*( const float& fl ) const {
		return { x * fl, y * fl, z * fl };
	}
	inline vec3_t operator/( const float& fl ) const {
		return { x / fl, y / fl, z / fl };
	}
	inline vec3_t& operator+=( const vec3_t& v ) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	inline vec3_t& operator-=( const vec3_t& v ) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	inline vec3_t& operator*=( const vec3_t& v ) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	inline vec3_t& operator/=( const vec3_t& v ) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}
	inline vec3_t& operator*=( const float& fl ) {
		x *= fl;
		y *= fl;
		z *= fl;
		return *this;
	}
	inline vec3_t& operator/=( const float& fl ) {
		x /= fl;
		y /= fl;
		z /= fl;
		return *this;
	}

	inline float length_sqr( ) const {
		return ( x * x ) + ( y * y ) + ( z * z );
	}
	inline float length( ) const {
		return std::sqrt( length_sqr( ) );
	}
	inline float length_2d_sqr( ) const {
		return ( x * x ) + ( y * y );
	}
	inline float length_2d( ) const {
		return std::sqrt( length_2d_sqr( ) );
	}

	inline void normalize( ) {
		*this /= length( );
	}
	inline vec3_t normalized( ) const {
		return *this / length( );
	}

	inline float dot( const vec3_t& v ) const {
		return ( x * v.x ) + ( y * v.y ) + ( z * v.z );
	}
	inline vec3_t cross( const vec3_t& v ) const {
		return {
			( y * v.z ) - ( z * v.y ),
			( z * v.x ) - ( x * v.z ),
			( x * v.y ) - ( y * v.x )
		};
	}
};
