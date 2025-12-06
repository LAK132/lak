# LAK

## License
This library is dual licensed under the Unlicense and MIT license. See LICENSE and UNLICENSE.
Attribution would be nice but is not required.

## Philosophy

* Reserving address space should not increase physical memory usage on a hosted system.
* Prefer dense allocation over sparse allocation.
* Recursion is evil.
* Abuse the type system whenever and wherever possible.
* Types should be either primarily data (POD/C-structs) or primarily functionality (containers, traits), but not both (OOP).
* `virtual` is _usually_ a sign of systematic design failure.
* Design APIs to be granular.

## Example program

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

#include <filesystem>

struct my_window : virtual public LAK_BASIC_PROGRAM(window_api)
{
	my_window() : LAK_BASIC_PROGRAM(window_api)() {}
	virtual ~my_window() {}

	// store any member variables needed by this window here so they can be
	// accessed during init/handle_event/loop

	lak::optional<std::filesystem::path> dropfile;

	virtual void init() override final
	{
		// called once window has been set up

		window().set_title(L"something other than " APP_NAME);
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

## Basic setup

Enable or disable features by modifying `meson_options.txt` or with `-D[feature]=[setting]` command line options during setup.

### Windows

```
> ./setup.bat msvc
> ./compile.bat
```

### WSL

```
> ./win_setup.sh msvc
> ./win_compile.sh
```

### Linux

```
> ./setup.sh gcc
> ./compile.sh
```

## Build with tests

```
> ./setup.bat msvc --buildtype=debug -Dlak_enable_tests=true
> ./compile.bat
```

## Build with libgphoto2

### Windows

Requires msys64

```
> ./setup.bat msvc -Dlak_enable_libgphoto2=true -Dlibgphoto2_msys_prefix=C:/msys64
> ./compile.bat install-msys-libgphoto2-dependencies
> ./compile.bat
```

### Other

Requires libgphoto2 dependencies to be preinstalled.

```
> ./setup.sh gcc -Dlak_enable_libgphoto2=true
> ./compile.sh
```

## Build with llvm

To use LLVM on Windows, MSVC must be installed with the MFC and ATL components

```
> ./setup.bat msvc -Dlak_enable_llvm=true
> ./compile.bat
```
