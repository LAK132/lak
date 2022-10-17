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
sdl2_dep = sdl2_subprj.dependency('SDL2_static')
sdl2_lib = sdl2_subprj.target('SDL2_static')

# There appears to be an issue with copying the headers into the build
# directory, if anything that depends on them is built too quickly then the
# compiler complains that it can't find some of the headers. inserting a delay
# in here seems to fix it
sdl2_build_delay = custom_target(
	'sdl2-dep-delay',
	output: 'sdl2-dep-delay.h',
	depends: sdl2_lib,
	command: [
		find_program('python'), '-c',
		'import time;time.sleep(5);open(\'@OUTPUT@\', \'w\').close()'
	],
)

lakwindowing = static_library(
	'lakwindowing',
	[
		'events.cpp',
		'platform.cpp',
		'window.cpp',
		sdl2_build_delay,
	],
	override_options: 'cpp_std=' + version,
	dependencies: [
		lakcore_dep,
		lakopengl_dep,
		sdl2_dep,
	],
)
