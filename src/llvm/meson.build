llvm_dep = subproject('llvm').get_variable('llvm_dep')

lakllvm = static_library(
	'lakllvm',
	[
		'llvm.cpp',
	],
	override_options: [
		'cpp_std=' + version,
		'warning_level=0',
		'werror=false',
	],
	dependencies: [
		lakcore_dep,
		llvm_dep,
	],
)

lakllvm_dep = declare_dependency(
	link_with: lakllvm,
	dependencies: llvm_dep,
)
