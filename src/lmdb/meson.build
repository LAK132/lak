laklmdb = static_library(
	'laklmdb',
	[
		'lmdb.cpp',
	],
	dependencies: [
		lakcore_dep,
		lmdb_dep,
	],
)

laklmdb_dep = declare_dependency(
	link_with: laklmdb,
	dependencies: lmdb_dep,
)
meson.override_dependency('lak-lmdb', laklmdb_dep)
