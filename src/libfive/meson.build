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
	override_options: override_options,
	dependencies: laklibfive_dependencies,
)

laklibfive_dep = declare_dependency(
	link_with: laklibfive,
	dependencies: laklibfive_dependencies.partial_dependency(
		includes: true, compile_args: true),
)
meson.override_dependency('lak-libfive', laklibfive_dep)
