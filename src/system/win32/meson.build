lakcore_system_win32 = {}
foreach type, comps : compilers
	if comps['machine'].system() == 'windows'
		lakcore_system_win32 += {type: static_library(
			'lakcore_system_win32-@0@'.format(type),
			[
				'architecture.cpp',
				'file.cpp',
				'page_memory.cpp',
				'process.cpp',
				'profile.cpp',
				'wrapper.cpp',
			],
			cpp_args: lak_reflection_argument,
			include_directories: lakcore_inc,
			dependencies: [],
			native: comps['native'],
		)}
	else
		lakcore_system_win32 += {type: []}
	endif
endforeach
