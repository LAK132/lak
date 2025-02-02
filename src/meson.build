lakcore_natvis = []
lakcore_linkargs = []
if cxx.get_id() == 'msvc'
	lakcore_natvis = meson.project_source_root()/'.natvis'
	lakcore_linkargs += '/NATVIS:@0@'.format(lakcore_natvis)
endif

lakcore = static_library(
	'lakcore',
	[
		'compression'/'deflate.cpp',
		'compression'/'lz4.cpp',
		'architecture.cpp',
		'bigint.cpp',
		'alloc.cpp',
		'debug.cpp',
		'file.cpp',
		'json.cpp',
		'math.cpp',
		'memmanip.cpp',
		'numeric.cpp',
		'process.cpp',
		'profile.cpp',
		'strconv.cpp',
		'string_view.cpp',
		'tasks.cpp',
		'test.cpp',
		'tinflate.cpp',
		'tokeniser.cpp',
		'unicode.cpp',
		'unique_pages.cpp',
	],
	override_options: [
		'cpp_std=' + version,
		'warning_level=3',
	],
	include_directories: include_directories('..', '..'/'include'),
	dependencies: dependency('threads'),
	link_args: lakcore_linkargs,
	link_depends: lakcore_natvis,
)

lakcore_dep = declare_dependency(
	link_with: lakcore,
	include_directories: include_directories('..', '..'/'include'),
	link_args: lakcore_linkargs,
)
