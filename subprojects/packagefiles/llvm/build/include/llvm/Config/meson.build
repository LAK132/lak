cmake_generated_headers = custom_target(
	'install-llvm-headers',
	depends: cmake_generated_llvm_c_lib,
	output: 'abi-breaking.h',
	console: true,
	command: [find_program('python', 'python3'), '-c', ''],
)
