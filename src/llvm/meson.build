if get_option('llvm_from_source')
	subproject('llvm-15.0.1')
endif

llvm_components = []

foreach target : get_option('llvm_components')
	llvm_components += target.to_lower()
endforeach

foreach target : get_option('llvm_install_components')
	llvm_components += target.to_lower()
endforeach

llvm_dep = dependency(
	'llvm',
	version: ['>=15', '<16'],
	modules: llvm_components,
	required: true,
	allow_fallback: true,
)

lakllvm = static_library(
	'lakllvm',
	[
		'llvm.cpp',
	],
	override_options: {
		'warning_level': '0',
	},
	dependencies: [
		lakcore_dep,
		llvm_dep,
	],
)

lakllvm_dep = declare_dependency(
	link_with: lakllvm,
)

meson.override_dependency('lak-llvm', lakllvm_dep)
