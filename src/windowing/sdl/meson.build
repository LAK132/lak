_lakwindowing_dependencies = [
	lakcore_dep,
	subproject('SDL2').get_variable('sdl2_dep'),
	dependency('dl', required: false),
]

if get_option('lak_enable_softrender')
	_lakwindowing_dependencies += laksoftrender_dep
endif

if get_option('lak_enable_opengl')
	_lakwindowing_dependencies += lakopengl_dep
endif

if get_option('lak_enable_vulkan')
	_lakwindowing_dependencies += lakvulkan_dep
endif

if get_option('lak_enable_metal')
	_lakwindowing_dependencies += lakmetal_dep
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
	override_options: 'cpp_std=' + version,
	dependencies: lakwindowing_dependencies,
)
