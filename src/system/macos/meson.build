lakcore_system_macos = {}
foreach type, comps : compilers
	if comps['machine'].system() == 'darwin'
		lakcore_system_macos += {type: static_library(
			'lakcore_system_macos-@0@'.format(type),
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
		lakcore_system_macos += {type: []}
	endif
endforeach
