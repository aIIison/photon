#pragma once

#include "sdk/platform.h"

class address_t {
protected:
	uintptr_t addr;

public:
	// default ctor/dtor.
	__forceinline address_t( ) : addr{ } { };
	__forceinline ~address_t( ){ };

	// ctors.
	__forceinline address_t( uintptr_t a ) : addr{ a } {}
	__forceinline address_t( const void* a ) : addr{ ( uintptr_t ) a } {}

	// arithmetic operators for native types.
	__forceinline operator uintptr_t( ) { return addr; }
	__forceinline operator void*( ) { return ( void* ) addr; }
	__forceinline operator const void*( ) { return ( const void* ) addr; }

	// is-equals-operator.
	__forceinline bool operator==( address_t a ) const {
		return as< uintptr_t >( ) == a.as< uintptr_t >( );
	}
	__forceinline bool operator!=( address_t a ) const {
		return as< uintptr_t >( ) != a.as< uintptr_t >( );
	}

	// cast / add offset and cast.
	template < typename t = address_t >
	__forceinline t as( ) const {
		return ( addr ) ? ( t ) addr : t{ };
	}

	template < typename t = address_t >
	__forceinline t as( size_t offset ) const {
		return ( addr ) ? ( t ) ( addr + offset ) : t{ };
	}

	template < typename t = address_t >
	__forceinline t as( ptrdiff_t offset ) const {
		return ( addr ) ? ( t ) ( addr + offset ) : t{ };
	}

	// add offset and dereference.
	template < typename t = address_t >
	__forceinline t at( size_t offset ) const {
		return ( addr ) ? *( t* ) ( addr + offset ) : t{ };
	}

	template < typename t = address_t >
	__forceinline t at( ptrdiff_t offset ) const {
		return ( addr ) ? *( t* ) ( addr + offset ) : t{ };
	}

	// add offset.
	template < typename t = address_t >
	__forceinline t add( size_t offset ) const {
		return ( addr ) ? ( t ) ( addr + offset ) : t{ };
	}

	template < typename t = address_t >
	__forceinline t add( ptrdiff_t offset ) const {
		return ( addr ) ? ( t ) ( addr + offset ) : t{ };
	}

	// subtract offset.
	template < typename t = address_t >
	__forceinline t sub( size_t offset ) const {
		return ( addr ) ? ( t ) ( addr - offset ) : t{ };
	}

	template < typename t = address_t >
	__forceinline t sub( ptrdiff_t offset ) const {
		return ( addr ) ? ( t ) ( addr - offset ) : t{ };
	}

	// dereference.
	template < typename t = address_t >
	__forceinline t to( ) const {
		return *( t* ) addr;
	}

	// dereference n times.
	template < typename t = address_t >
	__forceinline t get( size_t n = 1 ) {
		uintptr_t out;

		if ( !addr )
			return t{ };

		out = addr;

		for ( size_t i{ n }; i > 0; --i ) {
			// can't dereference, return null.
			if ( !out )
				return t{ };

			out = *( uintptr_t* ) out;
		}

		return reinterpret_cast< t& >( out );
	}

	// follow relative8 and relative16/32 offsets.
	template < typename t = address_t >
	__forceinline t rel8( size_t offset ) {
		uintptr_t out;
		uint8_t   r;

		if ( !addr )
			return t{ };

		out = addr + offset;

		// get relative offset.
		r = *( uint8_t* ) out;
		if ( !r )
			return t{ };

		// relative to address of next instruction.
		// short jumps can go forward and backward depending on the size of the second byte.
		// if the second byte is below 128, the jmp goes forwards.
		// if the second byte is above 128, the jmp goes backwards ( subtract two's complement of the relative offset from the address of the next instruction ).
		if ( r < 128 )
			out = ( out + 1 ) + r;
		else
			out = ( out + 1 ) - ( uint8_t ) ( ~r + 1 );

		return reinterpret_cast< t& >( out );
	}

	template < typename t = address_t >
	__forceinline t rel32( size_t offset ) {
		uintptr_t out;
		uint32_t  r;

		if ( !addr )
			return t{ };

		out = addr + offset;

		// get rel32 offset.
		r = *( uint32_t* ) out;
		if ( !r )
			return t{ };

		// relative to address of next instruction.
		out = ( out + 4 ) + r;

		return reinterpret_cast< t& >( out );
	}

	// set.
	template < typename t = uintptr_t >
	__forceinline void set( const t& value ) {
		if ( !addr )
			return;

		*( t* ) addr = value;
	}
};
