# <img src="resource/icon.png" width="23"/> Photon <!--- [![CI](https://github.com/aIIison/photon/actions/workflows/CI.yml/badge.svg)](https://github.com/aIIison/photon/actions/workflows/CI.yml) --> [![C++](https://img.shields.io/badge/language-C%2B%2B-f34b7d)](https://en.wikipedia.org/wiki/C%2B%2B) [![Portal 2](https://img.shields.io/badge/game-Portal%202-blue)](https://store.steampowered.com/app/620/Portal_2/) [![Platform](https://img.shields.io/badge/platform-Windows%20%26%20Linux-green)](https://en.wikipedia.org/wiki/Cross-platform_software)

**Photon** is a mod loader for Portal 2.

### Installation
- Download the newest release from [here](https://github.com/aIIison/photon/releases).
- Place it next to Portal 2's executable (typically in `SteamLibrary/steamapps/common/Portal 2`).
- Open the game and type `plugin_load photon` in the console.

### Mods
- Install mods by placing them in the `photon` folder inside the Portal 2 directory.
- Enable them from the menu that you can open by pressing <kbd>RShift</kbd>.

## Development

### Cloning the project
```
git clone --recurse-submodules https://github.com/aIIison/photon.git
```

### Building
Requires **CMake 3.15** or higher.

#### Windows
```
cmake -B build -A Win32
cmake --build build --config Debug/Release
```

#### Linux
```
cmake -B build
cmake --build build --config Debug/Release
```

> [!TIP]
> Configure paths in `cmake.toml` and run `cmake --install build` after building.  
> This automatically installs the binaries to Portal 2.

### Pull requests
- Follow the code style.
- Do not stage files that you had to configure.

### Code style
Use the included `.clang-format` file.

#### Naming
- `snake_case` for everything.
- `e_` prefix for enums.
- `c_` prefix for classes.
- `i_` prefix for interfaces.
- `u_` prefix for unions.
- `_t` suffix for data types.

###### Example
```cpp
enum class e_example {
	example_value
};

union u_example {
	int32_t i32;
	float   f32;
};

struct vec2_t {
	float x, y;
};

class i_example {
public:
	virtual void example_fn( ) = 0;
};

class c_example : public i_example {
public:
	virtual void example_fn( );
};
```

## Creating mods
> [!NOTE]
> There's an included [example mod](example-mod) with a few features you can take a look at.

Download and include the [SDK](src/sdk) in your project.

#### Barebones mod example
```cpp
/* mod.h */
#pragma once

#include <photon.h>

class c_example_mod : public photon::i_mod {
public:
	virtual bool               load( );
	virtual void               unload( );
	virtual void               on_event( const char* msg );
	virtual photon::mod_info_t get_info( );
	virtual void               paint_menu( photon::c_menu_context* ctx );
};

/* mod.cpp */
#include "mod.h"

EXPOSE_PHOTON_MOD( c_example_mod );

bool c_example_mod::load( ) {
	// allocate & initialize stuff here etc.

	photon::get( )->common->log("hey!\n");

	return true;
}

void c_example_mod::unload( ) {
	// unallocate & deinitialize stuff here etc.
}

void c_example_mod::on_event( const char* msg ) {
	// handle events here.
}

photon::mod_info_t c_example_mod::get_info( ) {
	return {
		.name    = "Example Mod",
		.author  = "hero",
		.version = "0.0.1"
	};
}

void c_example_mod::paint_menu( c_menu_context* ctx ) {
	// add controls to the menu here.
}
```
