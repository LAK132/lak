if target_machine.cpu_family() == 'x86'
	godot_cpp_build_bits = '32'
elif target_machine.cpu_family() == 'x86_64'
	godot_cpp_build_bits = '64'
elif target_machine.cpu_family() == 'arm'
	godot_cpp_build_bits = '32'
elif target_machine.cpu_family() == 'aarch64'
	godot_cpp_build_bits = '64'
endif

if target_machine.system() == 'windows'
	godot_cpp_output = 'godot-cpp.windows.@0@.@1@.lib'.format(godot_cpp_build_type.to_lower(), godot_cpp_build_bits)
elif target_machine.system() == 'darwin'
	godot_cpp_output = 'libgodot-cpp.darwin.@0@.@1@.a'.format(godot_cpp_build_type.to_lower(), godot_cpp_build_bits)
elif target_machine.system() == 'android'
	godot_cpp_output = 'godot-cpp.android.@0@.so'.format(godot_cpp_build_type.to_lower())
else
	godot_cpp_output = 'libgodot-cpp.@0@.@1@.@2@.a'.format(target_machine.system(), godot_cpp_build_type.to_lower(), godot_cpp_build_bits)
endif

cmake_generated_godot_cpp_lib = custom_target(
	'compile-godot-cpp',
	depends: godot_cpp_build,
	output: godot_cpp_output,
	console: true,
	command: [
		cmake,
		'--build', godot_cpp_build_dir,
	],
)
