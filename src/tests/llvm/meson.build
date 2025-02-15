laktestllvm = static_library(
	'laktestllvm',
	[
		'llvm.cpp',
	],
	override_options: override_options_w0,
	dependencies: [
		lakcore_dep,
		lakllvm_dep,
	],
)

laktestllvm_dep = declare_dependency(
	link_whole: laktestllvm,
)
