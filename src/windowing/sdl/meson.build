cmake = import('cmake')

sdl2_opts = cmake.subproject_options()

sdl2_opts.add_cmake_defines({
	'SDL_STATIC': 'ON',
	'SDL_SHARED': 'OFF',
	'SDL_DISABLE_UNINSTALL': 'ON',
	'SDL_DISABLE_INSTALL': 'ON',
})

sdl2_subprj = cmake.subproject('SDL2', options: sdl2_opts)

# sdl2_subprj.dependency('...'):
# SDL2
# SDL2-static
# SDL2_test
# SDL2main
# message('CMake targets:\n - ' + '\n - '.join(sdl2_subprj.target_list()))
sdl2_dep = sdl2_subprj.dependency('SDL2-static')

lakwindowing = static_library(
	'lakwindowing',
	[
		'events.cpp',
		'platform.cpp',
		'window.cpp',
	],
	override_options: 'cpp_std=' + version,
	dependencies: [
		lakcore_dep,
		lakopengl_dep,
		sdl2_dep,
	],
)
