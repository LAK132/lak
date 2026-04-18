lakcore_inc = include_directories('..', '..'/'include')

subdir('codegen')
subdir('compression')
subdir('dsl')
subdir('ebpf')
subdir('system')

lakcore2 = {}
lakcore_dep2 = {}
foreach type, comps : compilers
	lakcore2 += {type: static_library(
		'lakcore-@0@'.format(type),
		[
			lakcore_codegen_files,
			lakcore_compression_files,
			lakcore_dsl_files,
			lakcore_ebpf_files,
			'file'/'ebnf.cpp',
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
		include_directories: lakcore_inc,
		dependencies: dependency('threads'),
		link_with: lakcore_system[type],
		link_depends: lak_natvis,
		native: comps['native'],
	)}
	lakcore_dep2 += {type: declare_dependency(
		link_with: lakcore2[type],
		include_directories: lakcore_inc,
	)}
endforeach

lakcore = lakcore2['host']
lakcore_dep = lakcore_dep2['host']
