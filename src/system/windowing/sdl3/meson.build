# if get_option('sdl3_from_source')
# 	subproject('SDL3')
# 	sdl3_dep = dependency('SDL3')
# else
	sdl3_dep = dependency('SDL3')
# endif

_lakwindowing_dependencies = [
	lakcore_dep,
	glm_dep,
	sdl3_dep,
	dependency('dl', required: false),
]

if get_option('lak_renderer').contains('softrender')
	_lakwindowing_dependencies += laksoftrender_dep
endif

if get_option('lak_renderer').contains('opengl')
	_lakwindowing_dependencies += lakopengl_dep
endif

if get_option('lak_renderer').contains('cobalt')
	_lakwindowing_dependencies += lakcobalt_dep
endif

lakwindowing_dependencies = declare_dependency(
	dependencies: _lakwindowing_dependencies,
	compile_args: [
		'-DLAK_USE_SDL3',
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
	dependencies: lakwindowing_dependencies,
)
