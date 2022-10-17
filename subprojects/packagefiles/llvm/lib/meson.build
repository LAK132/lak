if target_machine.system() == 'windows'
	shared_lib_suffix = '.lib'
elif target_machine.system() == 'darwin'
	shared_lib_suffix = '.dylib'
else
	shared_lib_suffix = '.so'
endif

cmake_generated_llvm_c_lib = custom_target(
	'compile-llvm',
	depends: llvm_build,
	output: 'LLVM-C' + shared_lib_suffix,
	console: true,
	command: [
		cmake,
		'--build', llvm_build_dir,
		'--target', llvm_build_targets,
	],
)

llvm_c = static_library(
	'llvm',
	[
		cmake_generated_llvm_c_lib,
	],
)
