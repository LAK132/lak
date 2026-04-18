laktestlmdb = static_library(
	'laktestlmdb',
	[
		'lmdb.cpp',
	],
	override_options: {
		'warning_level': '0',
	},
	dependencies: [
		lakcore_dep,
		laklmdb_dep,
	],
)

laktestlmdb_dep = declare_dependency(
	link_whole: laktestlmdb,
)
