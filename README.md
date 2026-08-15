# LAK

Please make sure you have read the [basic setup guide](/README.md#basic-setup) before building anything!

- [License](/README.md#license)
- [Philosophy](/README.md#philosophy)
- [Basic setup](/README.md#basic-setup)
	- [Windows](/README.md#windows)
	- [Linux](/README.md#most-linux-distros)
		- [NixOS](/README.md#nixos)
	- [MacOS](/README.md#macos)
- [Tools](/README.md#tools)
	- [asc2csv](/README.md#asc2csv)
	- [ebnf2cpp](/README.md#ebnf2cpp)
	- [lisk-repl](/README.md#lisk-repl)
	- [mdc2png](/README.md#mdc2png)
- [Examples](/README.md#examples)
	- [basic_program](/README.md#basic_program)
	- [ball-game](/README.md#ball-game)
	- [hello-cobalt](/README.md#hello-cobalt)
	- [mdc-view](/README.md#mdc-view)
	- [nbt-view](/README.md#nbt-view)
- [Using `lak::`](/README.md#using-lak)
- [Advanced setup](/README.md#advanced-setup)
	- [Cross compiling](/README.md#cross-compiling)
		- [Emscripten](/README.md#emscripten)
	- [Tests](/README.md#build-with-tests)
	- [libgphoto2 support](/README.md#build-with-libgphoto2)

## License

This library is dual licensed under the Unlicense and MIT license. See LICENSE and UNLICENSE.
Attribution would be nice but is not required.

## Philosophy

- Reserving address space should not increase physical memory usage on a hosted system.
- Prefer dense allocation over sparse allocation.
- Recursion is evil.
- Runtime performance is more important than compile time performance.
- Abuse the type system whenever and wherever possible.
- Types should be either primarily data (POD/C-structs) or primarily functionality (containers, traits), but not both (OOP).
- `virtual` is _usually_ a sign of systematic design failure.
- Design APIs to be granular.

## Basic setup

Enable or disable features by modifying `meson_options.txt` or with `-D[feature]=[setting]` command line options during setup (`./setup.bat msvc -Dfeature=setting`, `./setup.sh gcc -Dfeature=setting`, etc, etc).

Running `setup.sh`/`setup.bat`/`win_setup.sh` multiple times will wipe the build directory, make sure you've specified all the options you need up front.

### Windows

Native (msvc) via command prompt:

```
./setup.bat msvc <feature settings>
./compile.bat <target>
```

Native (msvc) via WSL:

```
./win_setup.sh msvc <feature settings>
./win_compile.sh <target>
```

### MacOS

Install LLVM (at least `llvm@20` recommended) with homebrew

```
./setup.sh homebrew-clang <feature settings>
./compile.sh <target>
```

### Linux

#### `gcc`

```
./setup.sh gcc <feature settings>
./compile.sh <target>
```

#### `clang`

```
./setup.sh clang <feature settings>
./compile.sh <target>
```

#### NixOS

```
nix-shell
./setup.sh auto <feature settings>
./compile.sh <target>
```

## Tools

These are command line tools mostly designed to only depend on `std::` or a very minimal subset of the core of `lak::`, in some cases so they can be used as pre-processors for later build stages.

Run with no arguments to get help text.

### asc2csv

Converts `.asc` point cloud files to `.csv`s.

[Setup the build directory](/README.md#basic-setup) then
```
./compile.[sh/bat] asc2csv
./build/tools/asc2csv/asc2csv
```

### ebnf2cpp

Reads in a semi-custom EBNF grammar description language and uses it to generate `lak::dsl::` headers.

[Setup the build directory](/README.md#basic-setup) then
```
./compile.[sh/bat] ebnf2cpp
./build/tools/ebnf2cpp/ebnf2cpp
```

There is a generator in meson that can be used to use `ebnf2cpp` at compile time:

```meson
ebnf2cpp = subproject('lak').get_variable('ebnf2cpp')
my_lib = static_library(
	'my_lib',
	[
		'my_lib.cpp',
		# Accessed with #include "my_grammar.ebnf.hpp"
		ebnf2cpp.process('my_grammar.ebnf', extra_args: [
			'MY_LIB_EBNF_HPP', 'my_lib_parse']),
	],
)
```

### lisk-repl

[Setup the build directory](/README.md#basic-setup) then
```
./compile.[sh/bat] lisk-repl
./build/tools/lisk-repl/lisk-repl
```

```
lisk> (begin (define func (lambda (x n) (begin (if (zero? n) x (tail (func (* x 2) (- n 1))))))) (println (func 2 10)))
2048
lisk$ nil
lisk>
```

### mdc2png

Demosaics/desqueezes/upscales Minolta RD-175 `.MDC` raw files and saves the results as a `.png`.

[Setup requirements](/README.md#basic-setup):
- `-Dlak_enable_stb=true`
- `-Dlak_enable_stb_image_write=true`

[Setup the build directory](/README.md#basic-setup) then
```
./compile.[sh/bat] mdc2png
./build/tools/mdc2png/mdc2png /path/to/.MDC /path/to/.png
```

![mdc2png.png](/tools/mdc2png.png?raw=true)

## Examples

### basic_program

Demo of various features available with `<lak/basic_program.inl>`

[Setup requirements](/README.md#basic-setup):
- `-Dlak_enable_windowing=true`
- `-Dlak_enable_imgui=true`
- `-Dlak_renderer=`
	- `softrender`
	- `opengl`
	- `cobalt` (`-Dcobalt_renderer=` to change which Cobalt renderers to use)
	- any or all of the above (`-Dlak_renderer=softrender,opengl,cobalt`)

[Setup the build directory](/README.md#basic-setup) then
```
./compile.[sh/bat] basic_program
./build/examples/basic_program/basic_program
```

![basic_program.png](/examples/basic_program.png?raw=true)

### ball-game

Roll a ball around a maze of blocks and collect all the coins. Controlled with the arrow keys. Don't fall off!

[Setup requirements](/README.md#basic-setup):
- `-Dlak_enable_windowing=true`
- `-Dlak_enable_imgui=true`
- `-Dlak_renderer=opengl`

[Setup the build directory](/README.md#basic-setup) then
```
./compile.[sh/bat] ball-game
./build/examples/ball-game/ball-game
```

![ball-game.png](/examples/ball-game.png?raw=true)

Can load custom maps by dropping a `.pnm` image file into the game window.

- Red channel = ground blocks
- Green channel = collectable coins
- Blue channel = lights (max 32)

See [examples/ball-game/assets/map.ppm](/examples/ball-game/assets/map.ppm) for the example map.

### hello-cobalt

Demo of the Cobalt Renderer integration. Will attempt to open a window for every enabled renderer simultaneously.

[Setup requirements](/README.md#basic-setup):
- `-Dlak_enable_windowing=true`
- `-Dlak_enable_imgui=true`
- `-Dlak_renderer=cobalt`
- `-Dcobalt_renderer=` (optional: all system-available renderers enabled by default)
	- `OpenGL3` (Windows, MacOS, Linux)
	- `OpenGL4` (Windows, Linux)
	- `Vulkan` (Windows, MacOS, Linux)
	- `Direct3D11` (Windows)
	- `Direct3D12` (Windows)
	- any or all of the above (`-Dcobalt_renderer=OpenGL3,OpenGL4,Vulkan,Direct3D11,Direct3D12`)

[Setup the build directory](/README.md#basic-setup) then
```
./compile.[sh/bat] hello-cobalt
./build/examples/hello-cobalt/hello-cobalt
```

![hello-cobalt.png](/examples/hello-cobalt.png?raw=true)

### mdc-view

Inspector for Minolta RD-175 `.MDC` raw files.

[Setup requirements](/README.md#basic-setup):
- `-Dlak_enable_windowing=true`
- `-Dlak_enable_imgui=true`
- `-Dlak_enable_stb=true`
- `-Dlak_enable_stb_image_write=true`

[Setup the build directory](/README.md#basic-setup) then
```
./compile.[sh/bat] mdc-view
./build/examples/mdc-view/mdc-view
```

![mdc-view.png](/examples/mdc-view.png?raw=true)

### nbt-view

Inspector for `.nbt` files.

[Setup requirements](/README.md#basic-setup):
- `-Dlak_enable_windowing=true`
- `-Dlak_enable_imgui=true`
- `-Dlak_enable_stb=true`
- `-Dlak_enable_stb_image_write=true`

[Setup the build directory](/README.md#basic-setup) then
```
./compile.[sh/bat] nbt-view
./build/examples/nbt-view/nbt-view
```

![nbt-view.png](/examples/nbt-view.png?raw=true)

## Advanced setup

To specify compilers manually:

```
export CC=<C compiler>
export CXX=<C++ compiler>
export CC_FOR_BUILD=<C compiler>
export CXX_FOR_BUILD=<C++ compiler>
./setup.sh auto
./compile.sh <target>
```

MacOS example:

```
export CC=$(brew --prefix llvm@20)/bin/clang
export CXX=$(brew --prefix llvm@20)/bin/clang++
export CC_FOR_BUILD=$(brew --prefix llvm@20)/bin/clang
export CXX_FOR_BUILD=$(brew --prefix llvm@20)/bin/clang++
./setup.sh auto
./compile.sh <target>
```

Use `llvm@<version>` for a specific version of of LLVM. `llvm@20` or higher is recommended.

### Cross compiling

Pre-configured cross compilation options take the form:
```
./setup.sh <host system option> <build system option>
```

(Host system means the system the `<target>` executables will run on, build system means the system that will compile them. Some parts of `lak::` require building pre-processing tools, which means a compiler to build executables for the build machine must also be specificed)

#### `gcc` for host/`clang` for build

```
./setup.sh gcc clang
./compile.sh <target>
```

#### `clang` for host/`gcc` for build

```
./setup.sh clang gcc
./compile.sh <target>
```

#### Specify compilers manually

```
export CC_FOR_BUILD=<build C compiler>
export CXX_FOR_BUILD=<build C++ compiler>
./setup.sh auto --cross-file=<host system cross file>
./compile.sh <target>
```

```
export CC=<host C compiler>
export CXX=<host C++ compiler>
export CC_FOR_BUILD=<build C compiler>
export CXX_FOR_BUILD=<build C++ compiler>
./setup.sh auto
./compile.sh <target>
```

#### Emscripten

Specifying `wasm32` or `wasm64` will automatically select pre-configured cross files for use with Emscripten

```
./setup.sh wasm32 <build system option>
./compile.sh <target>
```

```
./setup.sh wasm64 <build system option>
./compile.sh <target>
```

### Build with tests

```
./setup.bat msvc --buildtype=debug -Dlak_enable_tests=true
./compile.bat lak_test
./build/lak_test --testall
```

### Build with libgphoto2

#### Windows

Requires msys64

```
./setup.bat msvc -Dlak_enable_libgphoto2=true -Dlibgphoto2_msys_prefix=C:/msys64
./compile.bat install-msys-libgphoto2-dependencies
./compile.bat <target>
```

#### Other

Requires libgphoto2 dependencies to be preinstalled.

```
./setup.sh gcc -Dlak_enable_libgphoto2=true
./compile.sh <target>
```

### Build with llvm

To use LLVM on Windows, MSVC must be installed with the MFC and ATL components

```
./setup.bat msvc -Dlak_enable_llvm=true
./compile.bat <target>
```

## Using `lak::`

An example of an extremely simple program that opens a fullscreen Dear ImGui
window (backend renderer determined by settings in `meson_options.txt`):

![basic_program.png](/basic_program.png?raw=true)

```cpp
// default window name and ID for root Dear ImGui window
#define APP_NAME "basic_program"

// handle Dear ImGui context creation, event processing and rendering
#define LAK_BASIC_PROGRAM_IMGUI_IMPL

// create a full screen Dear ImGui window surrounding calls to window loop
#define LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL

// can be included in other source/header files if access to these functions is
// needed from elsewhere (ensure config defines are set first)
// #include <lak/basic_program.hpp>

#include <lak/basic_program.inl> // include .inl file only once (typically your
                                 // main.cpp or equivalent)

#include <lak/optional.hpp>

#include <lak/imgui/texture.hpp>

#include <filesystem>

// refer to basic_program.inl for an example of how to implement main yourself
// and avoid the dynamic dispatch of LAK_BASIC_PROGRAM(window_api)
struct my_window : virtual public LAK_BASIC_PROGRAM(window_api)
{
	my_window() : LAK_BASIC_PROGRAM(window_api)() {}

	// store any member variables needed by this window here so they can be
	// accessed during init/handle_event/loop

	lak::optional<std::filesystem::path> dropfile;

	// unique_com_ptr wrapper of ImTextureRef which will automatically handle
	// destroying the texture
	lak::ImUniqueTexture checker;

	virtual void init() override final
	{
		// called once window has been set up

		window().set_title(L"something other than " APP_NAME);

		lak::image<lak::vec3u8_t> checker_img;
		checker_img.resize(lak::vec2s_t{30U, 30U});
		for (size_t y = 0U; y < checker_img.size().y; ++y)
			for (size_t x = 0U; x < checker_img.size().x; ++x)
				checker_img[{x, y}].r = checker_img[{x, y}].g = checker_img[{x, y}].b =
				  (((x + y) & 1U) * 255U);
		checker.emplace(checker_img); // see also: lak::CreateTexture() and
		                              // lak::DestroyTexture()
	}

	virtual ~my_window()
	{
		// handle end-of-window-lifetime cleanup here
	}

	virtual void handle_event(lak::event &event) override final
	{
		// called whenever the window receives an event
		switch (event.type)
		{
			case lak::event_type::close_window:
				destroy(); // window is added to the destroy queue (flushed at end of
				           // the current frame)
				break;
			case lak::event_type::dropfile:
				dropfile = event.dropfile().path;
				break;
		}
	}

	virtual void loop(uint64_t counter_delta) override final
	{
		// called once every frame

		ImGui::Text("Frame time: %01.2fms",
		            ((float)counter_delta * 1000U) / lak::performance_frequency());

		lak::window &wnd = window();
		ImGui::Text("Window size: %lu x %lu", wnd.size().x, wnd.size().y);
		ImGui::Text("Drawable size: %lu x %lu",
		            wnd.drawable_size().x,
		            wnd.drawable_size().y);

		if_let_some (auto &path, dropfile)
			ImGui::Text("Dropped file: %s", path.generic_string().c_str());

		ImGui::Image(checker, ImVec2(200, 200));
	}
};

// lak::error_code<int> -> lak::result<lak::monostate, int>
lak::error_code<int> LAK_BASIC_PROGRAM(program_preinit)(lak::span<char *> args)
{
	// called at program startup

	// return lak::ok_t{}: continue onto program_init
	// return lak::err_t{int}: quit program with that exit code
	return lak::ok_t{};
}

lak::weak_ptr<LAK_BASIC_PROGRAM(window_instance<my_window>)> my_window_ptr;

lak::error_code<int> LAK_BASIC_PROGRAM(program_init)()
{
	// called after program_preinit and once all platform initialisation is
	// complete. you can start creating windows now

	auto map_str_err = [](lak::u8string err) -> int
	{
		ERROR(err);
		return EXIT_FAILURE;
	};

	// try macros can be used thanks to the result type return value
	RES_TRY_ASSIGN(
	  my_window_ptr =,
	  LAK_BASIC_PROGRAM(create_window<my_window>)().map_err(map_str_err));
	// by not specifying a specific graphics settings struct, create_window will
	// attempt to find the first working graphics backend (settings for each are
	// pulled from the global LAK_BASIC_PROGRAM(window_*_settings) structs).

	DEBUG_EXPR(my_window_ptr.get()->window().graphics());

	// return lak::ok_t{}: continue onto program_loop
	// return lak::err_t{int}: quit program with that exit code
	return lak::ok_t{};
}

void LAK_BASIC_PROGRAM(program_handle_event)(lak::event &event)
{
	// handle non-window events
}

bool LAK_BASIC_PROGRAM(program_loop)(uint64_t counter_delta)
{
	// called once per frame

	// return true: continue program execution
	// return false: stop main program loop, continues to program_quit
	return !LAK_BASIC_PROGRAM(window_instances)().empty();
}

int LAK_BASIC_PROGRAM(program_quit)()
{
	// called after program_loop has returned false and after basic_program's
	// window bank has been cleared (if you are holding onto any window
	// pointers, clear them here)

	my_window_ptr.reset(); // this was only a weak pointer, so the window should
	                       // have already been destroyed, but this will still
	                       // deallocate the memory now

	// return value used as program exit code
	return EXIT_SUCCESS;
}
```

`lak::` has been developed with a granular API, so if you find this
(basic_program) style of application framework too restricting,
you might look at the source of basic_program.hpp/basic_program.inl
for examples of how you might implement a lower level program yourself.
