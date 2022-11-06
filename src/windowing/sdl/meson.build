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

sdl2_dep = declare_dependency(
	dependencies: sdl2_subprj.dependency('SDL2_static'),

	# the meson cmake module isn't picking up on the fact that the SDL2_static
	# target is generating headers that later build steps depend on. so here we
	# introduce a custom_target which depends on the SDL2_static lib, and
	# generates a header which is marked as a source file for any later build
	# steps that depen on sdl2_dep
	sources: custom_target(
		'sdl2-dep-delay',
		output: 'sdl2-dep-delay.hpp',
		input: sdl2_subprj.target('SDL2_static'),
		capture: true,
		command: [find_program('python'), '-c', 'print("")'],
	),
)

lakwindowing_dependencies = declare_dependency(
	dependencies: [
		lakcore_dep,
		lakopengl_dep,
		sdl2_dep,
	],
)

lakwindowing = static_library(
	'lakwindowing',
	[
		'events.cpp',
		'platform.cpp',
		'window.cpp',
	],
	override_options: 'cpp_std=' + version,
	dependencies: lakwindowing_dependencies,
)
