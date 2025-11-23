subproject('SDL2-2.32.10')

_lakwindowing_dependencies = [
	lakcore_dep,
	dependency('SDL2-2.32.10'),
	dependency('dl', required: false),
]

if get_option('lak_enable_softrender')
	_lakwindowing_dependencies += laksoftrender_dep
endif

if get_option('lak_enable_opengl')
	_lakwindowing_dependencies += lakopengl_dep
endif

lakwindowing_dependencies = declare_dependency(
	dependencies: _lakwindowing_dependencies,
)

lakwindowing = static_library(
	'lakwindowing',
	[
		'events.cpp',
		'platform.cpp',
		'window.cpp',
	],
	override_options: override_options,
	dependencies: lakwindowing_dependencies,
)
