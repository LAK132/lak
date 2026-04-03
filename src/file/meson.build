lakfile = static_library(
	'lakfile',
	[
		'json.cpp',
		'pnm.cpp',
	],
	override_options: override_options,
	dependencies: [
		lakcore_dep,
	],
)

lakfile_dep = declare_dependency(
	link_with: lakfile,
	dependencies: [
	],
)
