lakcore_system_emscripten = {}
foreach type, comps : compilers
	if comps['machine'].system() == 'emscripten'
		lakcore_system_emscripten += {type: static_library(
			'lakcore_system_emscripten-@0@'.format(type),
			[
				'architecture.cpp',
				'dll.cpp',
				'file.cpp',
				'page_memory.cpp',
				'process.cpp',
				'profile.cpp',
			],
			cpp_args: lak_reflection_argument,
			include_directories: lakcore_inc,
			dependencies: [],
			native: comps['native'],
		)}
	else
		lakcore_system_emscripten += {type: []}
	endif
endforeach
