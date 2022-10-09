laktestlmdb = static_library(
	'laktestlmdb',
	[
		'lmdb.cpp',
	],
	override_options: [
		'cpp_std=' + version,
		'warning_level=0',
		'werror=false',
	],
	dependencies: [
		lakcore_dep,
		laklmdb_dep,
	],
)
