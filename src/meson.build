lakcore_natvis = []
lakcore_linkargs = []
if cxx.get_id() == 'msvc'
	lakcore_natvis = meson.project_source_root()/'.natvis'
	lakcore_linkargs += '/NATVIS:@0@'.format(lakcore_natvis)
endif

lakcore_inc = include_directories('..', '..'/'include')

subdir('codegen')
subdir('compression')
subdir('ebpf')
subdir('file')
subdir('system')

lakcore = static_library(
	'lakcore',
	[
		lakcore_codegen_files,
		lakcore_compression_files,
		lakcore_ebpf_files,
		lakcore_file_files,
		'bigint.cpp',
		'alloc.cpp',
		'debugger.cpp',
		'math.cpp',
		'memmanip.cpp',
		'numeric.cpp',
		'runtime_type.cpp',
		'strconv.cpp',
		'string_view.cpp',
		'tasks.cpp',
		'test.cpp',
		'tinflate.cpp',
		'tokeniser.cpp',
		'unicode.cpp',
	],
	override_options: override_options_w3,
	include_directories: lakcore_inc,
	dependencies: dependency('threads'),
	link_with: lakcore_system,
	link_args: lakcore_linkargs,
	link_depends: lakcore_natvis,
)

lakcore_dep = declare_dependency(
	link_with: lakcore,
	include_directories: lakcore_inc,
	link_args: lakcore_linkargs,
)
