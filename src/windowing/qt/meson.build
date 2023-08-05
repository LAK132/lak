_lakwindowing_dependencies = [
	lakcore_dep,
	dependency('qt5', modules: ['Core', 'Widgets'], required: true),
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
