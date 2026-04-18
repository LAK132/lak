lakgodot = static_library(
	'lakgodot',
	[
		'godot.cpp',
	],
	dependencies: [
		lakcore_dep,
		godot_cpp_dep,
	],
)

lakgodot_dep = declare_dependency(
	link_with: lakgodot,
	dependencies: godot_cpp_dep,
)
meson.override_dependency('lak-godot', lakgodot_dep)
