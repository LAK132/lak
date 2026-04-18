lakcore_system_linux = {}
foreach type, comps : compilers
	if comps['machine'].system() == 'linux'
		lakcore_system_linux += {type: static_library(
			'lakcore_system_linux-@0@'.format(type),
			[
				'architecture.cpp',
				'file.cpp',
				'process.cpp',
				'profile.cpp',
			],
			include_directories: lakcore_inc,
			dependencies: [],
			native: comps['native'],
		)}
	else
		lakcore_system_linux += {type: []}
	endif
endforeach
