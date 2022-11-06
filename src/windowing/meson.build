subdir(get_option('lak_backend'))

lakwindowing_dep = declare_dependency(
	link_with: lakwindowing,
	dependencies: lakwindowing_dependencies.partial_dependency(
		includes: true, compile_args: true, sources: true),
)
