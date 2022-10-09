lakcore = static_library(
	'lakcore',
	[
		'architecture.cpp',
		'compression/deflate.cpp',
		'compression/lz4.cpp',
		'alloc.cpp',
		'debug.cpp',
		'file.cpp',
		'json.cpp',
		'math.cpp',
		'memmanip.cpp',
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
	include_directories: include_directories('..', '../include'),
	dependencies: dependency('threads'),
)

lakcore_dep = declare_dependency(
	link_with: lakcore,
	include_directories: include_directories('..', '../include'),
)
