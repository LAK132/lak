laktestlibfive = static_library(
	'laktestlibfive',
	[
		'libfive.cpp',
	],
	override_options: {
		'warning_level': '0',
	},
	dependencies: [
		lakcore_dep,
		laklibfive_dep,
	],
)

laktestlibfive_dep = declare_dependency(
	link_whole: laktestlibfive,
)
