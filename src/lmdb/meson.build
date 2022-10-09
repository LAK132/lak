lmdb_dep = subproject('lmdb').get_variable('lmdb_dep')

laklmdb = static_library(
	'laklmdb',
	[
		'lmdb.cpp',
	],
	override_options: 'cpp_std=' + version,
	dependencies: [
		lakcore_dep,
		lmdb_dep,
	],
)

laklmdb_dep = declare_dependency(
	link_with: laklmdb,
	dependencies: lmdb_dep,
)
