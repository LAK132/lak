project(
	'imgui_club',
	'cpp',
	license: 'MIT',
	license_files: 'LICENSE.txt',
)

imgui_club_libraries = {
	'imgui_memory_editor': {
		'version': '0.59',
	},
	'imgui_multicontext_compositor': {
		'version': '0.11',
	},
	'imgui_threaded_rendering': {
		'version': '0.10',
	},
}

foreach library, info : imgui_club_libraries
	dep = declare_dependency(
		include_directories: library,
		version: info.get('version'),
	)
	meson.override_dependency(
		'@0@-@1@'.format(library, info.get('version')), dep)
	meson.override_dependency(library, dep)
endforeach
