_lakwindowing_dependencies = [
	lakcore_dep,
	glm_dep,
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
