#include "sdk/config.h"

#include "core/configs/configs.h"

void photon::c_config::begin( const char* module_name ) { configs::begin( module_name ); }
void photon::c_config::end( ) { configs::end( ); }

void photon::c_config::serialize( const char* var_name, const char*& var ) { configs::serialize( var_name, var ); }
void photon::c_config::serialize( const char* var_name, bool& var ) { configs::serialize( var_name, var ); }
void photon::c_config::serialize( const char* var_name, int32_t& var ) { configs::serialize( var_name, var ); }
void photon::c_config::serialize( const char* var_name, uint32_t& var ) { configs::serialize( var_name, var ); }
void photon::c_config::serialize( const char* var_name, float& var ) { configs::serialize( var_name, var ); }
void photon::c_config::serialize( const char* var_name, color_t& var ) { configs::serialize( var_name, var ); }

void photon::c_config::deserialize( const char* module_name ) { configs::deserialize( module_name ); }
