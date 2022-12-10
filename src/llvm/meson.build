llvm_subprj = subproject('llvm')
llvm_dep = llvm_subprj.get_variable('llvm_dep')

# :HACK: meson appears to have some strange dependency management issues going
# on with custom targets and static libraries
llvm_headers = llvm_subprj.get_variable('llvm_cmake_generated_headers')

lakllvm = static_library(
	'lakllvm',
	[
		'llvm.cpp',
		llvm_headers,
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
)
