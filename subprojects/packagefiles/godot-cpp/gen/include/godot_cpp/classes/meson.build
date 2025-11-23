godot_cpp_build_dep = custom_target(
	'godot-cpp-dep',
	output: 'node.hpp',
	depends: cmake_generated_godot_cpp_lib,
	capture: false,
	command: [
		import('python').find_installation(), '-c', '',
	],
)
