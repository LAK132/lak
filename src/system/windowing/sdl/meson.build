if get_option('sdl2_from_source')
	subproject('SDL2-2.32.10')
	sdl2_dep = dependency('SDL2-2.32.10')
else
	sdl2_dep = dependency('SDL2')
endif

_lakwindowing_dependencies = [
	lakcore_dep,
	glm_dep,
	sdl2_dep,
	dependency('dl', required: false),
]

if get_option('lak_enable_softrender')
	_lakwindowing_dependencies += laksoftrender_dep
endif

if get_option('lak_enable_opengl')
	_lakwindowing_dependencies += lakopengl_dep
endif

if get_option('lak_enable_cobalt')
	_lakwindowing_dependencies += lakcobalt_dep
endif

lakwindowing_dependencies = declare_dependency(
	dependencies: _lakwindowing_dependencies,
	compile_args: [
		'-DLAK_USE_SDL',
		'-DSDL_MAIN_HANDLED',
	],
)

_lakwindowing = [
	'events.cpp',
	'platform.cpp',
	'window.cpp',
]

if host_machine.system() == 'darwin'
	_lakwindowing += 'window.mm'
endif

lakwindowing = static_library(
	'lakwindowing',
	_lakwindowing,
	override_options: {
		# a system header on macos is doing something that is illegal in later
		# versions of C++, so limit to C++20 where it is merely deprecated
		'cpp_std': 'c++20',
	},
	dependencies: lakwindowing_dependencies,
)
