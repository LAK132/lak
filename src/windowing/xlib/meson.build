lakwindowing_dependencies = declare_dependency(
	dependencies: [
		lakcore_dep,
		lakopengl_dep,
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
