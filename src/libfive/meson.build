libfive_dep = subproject('libfive').get_variable('libfive_dep')

laklibfive_dependencies = declare_dependency(
	dependencies: [
		lakcore_dep,
		libfive_dep,
	],
)

laklibfive = static_library(
	'laklibfive',
	[
		'libfive.cpp',
	],
	override_options: 'cpp_std=' + version,
	dependencies: laklibfive_dependencies,
)

laklibfive_dep = declare_dependency(
	link_with: laklibfive,
	dependencies: laklibfive_dependencies.partial_dependency(
		includes: true, compile_args: true),
)
