godot_cpp_dep = subproject('godot-cpp-4.1').get_variable('godot_cpp_dep')

lakgodot = static_library(
	'lakgodot',
	[
		'godot.cpp',
	],
	override_options: 'cpp_std=' + version,
	dependencies: [
		lakcore_dep,
		godot_cpp_dep,
	],
)

lakgodot_dep = declare_dependency(
	link_with: lakgodot,
	dependencies: godot_cpp_dep,
)
