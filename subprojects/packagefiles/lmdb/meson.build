project(
	'lmdb',
	'c',
)

if target_machine.system() == 'windows'
	add_project_arguments('-DNTSTATUS=LONG', language: 'c')
endif

includes = include_directories('libraries/liblmdb')

lmdb = static_library(
	'lmdb',
	[
		'libraries/liblmdb/mdb.c',
		'libraries/liblmdb/midl.c',
	],
	override_options: [
		'warning_level=0',
	],
	include_directories: includes,
)

lmdb_dep = declare_dependency(
	link_with: lmdb,
	include_directories: includes,
)

meson.override_dependency('lmdb', lmdb_dep)
