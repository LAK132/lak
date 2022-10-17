laktestwindowing = static_library(
	'laktestwindowing',
	[
		'basic_program.cpp',
		'window.cpp',
	],
	override_options: [
		'cpp_std=' + version,
		'warning_level=2',
		'werror=false',
	],
	dependencies: [
		lakcore_dep,
		lakwindowing_dep,
	],
)

laktestwindowing_dep = declare_dependency(
	link_whole: laktestwindowing,
)
