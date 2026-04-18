laktestllvm = static_library(
	'laktestllvm',
	[
		'llvm.cpp',
	],
	override_options: {
		'warning_level': '0',
	},
	dependencies: [
		lakcore_dep,
		lakllvm_dep,
	],
)

laktestllvm_dep = declare_dependency(
	link_whole: laktestllvm,
)
