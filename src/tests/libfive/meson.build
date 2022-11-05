laktestlibfive = static_library(
	'laktestlibfive',
	[
		'libfive.cpp',
	],
	override_options: [
		'cpp_std=' + version,
		'warning_level=0',
		'werror=false',
	],
	dependencies: [
		lakcore_dep,
		laklibfive_dep,
	],
)

laktestlibfive_dep = declare_dependency(
	link_whole: laktestlibfive,
)
