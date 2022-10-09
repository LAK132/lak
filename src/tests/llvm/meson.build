laktestllvm = static_library(
	'laktestllvm',
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
		lakllvm_dep,
	],
)
