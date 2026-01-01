#include "sdk/signal.h"

#include "sdk/photon.h"
#include "util/util.h"

#include <dynohook/conventions/x86_ms_thiscall.h>
#include <dynohook/manager.h>
#include <unordered_map>

static std::unordered_map< std::string, signal_builder_t* > signals;

static const char* module_name;

struct signal_t {
	void*                      addr;
	e_data_type                return_type{ };
	e_callconv                 callconv{ };
	std::vector< e_data_type > params{ };
};

signal_builder_t* signal_builder_t::with_return_type( e_data_type type ) {
	auto signal         = reinterpret_cast< signal_t* >( this->signal );
	signal->return_type = type;
	return this;
}
signal_builder_t* signal_builder_t::with_callconv( e_callconv callconv ) {
#ifdef _WIN32
	auto signal      = reinterpret_cast< signal_t* >( this->signal );
	signal->callconv = callconv;
#endif
	return this;
}
signal_builder_t* signal_builder_t::with_parameters( const std::vector< e_data_type >& params ) {
	auto signal    = reinterpret_cast< signal_t* >( this->signal );
	signal->params = params;
	return this;
}
signal_builder_t* signal_builder_t::in_module( const char* name ) {
	auto signal  = reinterpret_cast< signal_t* >( this->signal );
	module_name  = name;
	signal->addr = photon->common->get_module_handle( module_name );
	return this;
}
signal_builder_t* signal_builder_t::in_interface( const char* name ) {
	auto signal  = reinterpret_cast< signal_t* >( this->signal );
	signal->addr = photon->common->get_interface( module_name, name );
	return this;
}
signal_builder_t* signal_builder_t::at_address( void* address ) {
	auto signal  = reinterpret_cast< signal_t* >( this->signal );
	signal->addr = address;
	return this;
}
signal_builder_t* signal_builder_t::from_vtable( size_t index ) {
	auto signal  = reinterpret_cast< signal_t* >( this->signal );
	signal->addr = reinterpret_cast< void* >( ( *reinterpret_cast< int** >( signal->addr ) )[ index ] );
#ifdef _WIN32
	signal->callconv = Thiscall;
#endif
	return this;
}
signal_builder_t* signal_builder_t::from_pattern( const char* pattern ) {
	auto signal  = reinterpret_cast< signal_t* >( this->signal );
	signal->addr = photon->common->pattern_scan( module_name, pattern );
	return this;
}
signal_builder_t* signal_builder_t::add_callback( e_callback_type type, void* fn ) {
	auto signal = reinterpret_cast< signal_t* >( this->signal );
	dyno::HookManager::Get( ).findDetour( signal->addr )->addCallback( ( dyno::CallbackType ) type, ( dyno::CallbackHandler ) fn );
	return this;
}
signal_builder_t* signal_builder_t::remove_callback( e_callback_type type, void* fn ) {
	auto signal = reinterpret_cast< signal_t* >( this->signal );
	dyno::HookManager::Get( ).findDetour( signal->addr )->removeCallback( ( dyno::CallbackType ) type, ( dyno::CallbackHandler ) fn );
	return this;
}
signal_builder_t* signal_builder_t::enable( ) {
	photon->signal->enable( this );
	return this;
}
signal_builder_t* signal_builder_t::disable( ) {
	photon->signal->disable( this );
	return this;
}

signal_builder_t* c_signal::create( const char* name ) {
	auto signal    = new signal_builder_t( );
	signal->signal = new signal_t( );
	signals.insert( std::make_pair( std::string( name ), signal ) );
	return signal;
}
void c_signal::remove( const char* name ) {
	auto signal = get( name );
	signal->disable( );
	delete signal->signal;
	delete signal;
	signals.erase( name );
}
void c_signal::enable( signal_builder_t* signal ) {
	auto data = reinterpret_cast< signal_t* >( signal->signal );

	std::vector< dyno::DataObject > params;

	dyno::ConvFunc fn;
	std::transform( data->params.begin( ),
	                data->params.end( ),
	                std::back_inserter( params ),
	                []( e_data_type x ) { return ( dyno::DataType ) x; } );
	switch ( data->callconv ) {
	case Cdecl:
		fn = [ & ]( ) { return new dyno::x86MsCdecl( params, ( dyno::DataType ) data->return_type ); };
		break;
	case Thiscall:
		fn = [ & ]( ) { return new dyno::x86MsThiscall( params, ( dyno::DataType ) data->return_type ); };
		break;
	case Stdcall:
		fn = [ & ]( ) { return new dyno::x86MsStdcall( params, ( dyno::DataType ) data->return_type ); };
		break;
	};

	auto hook = dyno::HookManager::Get( ).hookDetour( data->addr, fn );

	/* logging */
	for ( auto it = signals.begin( ); it != signals.end( ); ++it ) {
		if ( it->second == signal ) {
			auto name = it->first;

			if ( !hook ) {
				util::console::log_error( "[!] failed to enable signal %s [%p].\n", name.c_str( ), data->addr );
				return;
			}

			auto type_str = []( e_data_type x ) {
				return std::vector< std::string >{
					"void",
					"bool",
					"int8_t",
					"uint8_t",
					"int16_t",
					"uint16_t",
					"int32_t",
					"uint32_t",
					"int64_t",
					"uint64_t",
					"float",
					"double",
					"void*",
					"char*",
					"wchar_t*",
					"__m128",
					"__m256",
					"__m512",
					"object"  // ?
				}[ x ];
			};
			auto conv_str = []( e_callconv x ) {
				return std::vector< std::string >{
					"__cdecl",
					"__stdcall",
					"__thiscall"
				}[ x ];
			};

			std::string fn_sig;
			fn_sig += PRINT_MAGENTA + type_str( data->return_type ) + " ";
			fn_sig += PRINT_BLUE + conv_str( data->callconv ) + PRINT_WHITE " ";
			fn_sig += name;

			fn_sig += "( ";
			for ( auto param : data->params ) {
				fn_sig += PRINT_MAGENTA + type_str( param ) + PRINT_WHITE ", ";
			}
			fn_sig.pop_back( );
			fn_sig.pop_back( );
			fn_sig += " )" PRINT_RESET;

			util::console::log( "[+] enabled signal %s " PRINT_CYAN "[%p]" PRINT_RESET ".\n", fn_sig.c_str( ), data->addr );
		}
	}
}
void c_signal::disable( signal_builder_t* signal ) {
	auto data = reinterpret_cast< signal_t* >( signal->signal );
	dyno::HookManager::Get( ).unhookDetour( data->addr );

	/* logging */
	for ( auto it = signals.begin( ); it != signals.end( ); ++it ) {
		if ( it->second == signal ) {
			auto name = it->first;

			util::console::log( "[-] disabled signal" PRINT_YELLOW " %s " PRINT_CYAN "[%p]" PRINT_RESET ".\n", name.c_str( ), data->addr );
		}
	}
}
signal_builder_t* c_signal::get( const char* name ) {
	return signals[ name ];
}
void c_signal::remove_all( ) {
	dyno::HookManager::Get( ).unhookAll( );
	signals.clear( );
}

void* c_signal::get_arg( signal_context_t* ctx, size_t index ) {
	auto hook = reinterpret_cast< dyno::IHook* >( ctx );
	return hook->getArgument< void* >( index );
}
void c_signal::set_arg( signal_context_t* ctx, size_t index, void* value ) {
	auto hook = reinterpret_cast< dyno::IHook* >( ctx );
	return hook->setArgument< void* >( index, value );
}
void* c_signal::get_return( signal_context_t* ctx ) {
	auto hook = reinterpret_cast< dyno::IHook* >( ctx );
	return hook->getReturn< void* >( );
}
void c_signal::set_return( signal_context_t* ctx, void* value ) {
	auto hook = reinterpret_cast< dyno::IHook* >( ctx );
	return hook->setReturn< void* >( value );
}
