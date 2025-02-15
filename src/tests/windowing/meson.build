laktestwindowing = static_library(
	'laktestwindowing',
	[
		'basic_program.cpp',
		'window.cpp',
	],
	override_options: override_options_w2,
	dependencies: [
		lakcore_dep,
		lakwindowing_dep,
	],
)

laktestwindowing_dep = declare_dependency(
	link_whole: laktestwindowing,
)
