gl3w = static_library(
	'gl3w',
	'gl3w.c',
	override_options: 'warning_level=2',
	include_directories: include_directories('../../include'),
)

lakopengl = static_library(
	'lakopengl',
	[
		'mesh.cpp',
		'shader.cpp',
		'texture.cpp',
	],
	override_options: 'cpp_std=' + version,
	link_with: [
		gl3w,
	],
	dependencies: [
		lakcore_dep,
		dependency('threads'),
	],
)

lakopengl_dep = declare_dependency(
	link_with: lakopengl,
)
