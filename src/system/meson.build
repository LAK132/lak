lakcore_system_common = {}
foreach type, comps : compilers
	lakcore_system_common += {type: static_library(
		'lakcore_system_common-@0@'.format(type),
		[
			'architecture.cpp',
			'file.cpp',
			'page_memory.cpp',
			'profile.cpp',
		],
		include_directories: lakcore_inc,
		dependencies: [],
		native: comps['native'],
	)}
endforeach

subdir('emscripten')
subdir('posix')
subdir('linux')
subdir('macos')
subdir('win32')

lakcore_system = {}
foreach type, comps : compilers
	lakcore_system += {type: [
		lakcore_system_common[type],
		lakcore_system_emscripten[type],
		lakcore_system_posix[type],
		lakcore_system_linux[type],
		lakcore_system_macos[type],
		lakcore_system_win32[type],
	]}
endforeach
