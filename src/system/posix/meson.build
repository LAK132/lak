lakcore_system_posix = {}
foreach type, comps : compilers
	if comps['machine'].system() == 'linux' or comps['machine'].system() == 'darwin'
		lakcore_system_posix += {type: static_library(
			'lakcore_system_posix-@0@'.format(type),
			[
				'dll.cpp',
				'file.cpp',
				'page_memory.cpp',
				'wrapper.cpp',
			],
			cpp_args: lak_reflection_argument,
			include_directories: lakcore_inc,
			dependencies: [],
			native: comps['native'],
		)}
	else
		lakcore_system_posix += {type: []}
	endif
endforeach
