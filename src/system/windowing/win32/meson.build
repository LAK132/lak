_lakwindowing_dependencies = [
	lakcore_dep,
	glm_dep,
]

if get_option('lak_enable_softrender')
	_lakwindowing_dependencies += laksoftrender_dep
endif

if get_option('lak_enable_opengl')
	_lakwindowing_dependencies += [
		lakopengl_dep,
		dependency('GL'),
	]
endif

if get_option('lak_enable_cobalt')
	_lakwindowing_dependencies += lakcobalt_dep
endif

lakwindowing_dependencies = declare_dependency(
	dependencies: _lakwindowing_dependencies,
	compile_args: '-DLAK_USE_WINAPI',
)

lakwindowing = static_library(
	'lakwindowing',
	[
		'events.cpp',
		'platform.cpp',
		'window.cpp',
	],
	dependencies: lakwindowing_dependencies,
)
