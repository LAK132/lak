libfive_dep = subproject('libfive').get_variable('libfive_dep')

laklibfive = static_library(
	'laklibfive',
	[
		'libfive.cpp',
	],
	dependencies: [
		lakcore_dep,
		libfive_dep,
	]
)

laklibfive_dep = declare_dependency(
	link_with: laklibfive,
	dependencies:
		libfive_dep.partial_dependency(includes: true, compile_args: true),
)
