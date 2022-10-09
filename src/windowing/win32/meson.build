lakwindowing = static_library(
	'lakwindowing',
	[
		'events.cpp',
		'platform.cpp',
		'window.cpp',
	],
	override_options: 'cpp_std=' + version,
	dependencies: [
		lakcore_dep,
		lakopengl_dep,
		dependency('GL'),
	],
)
