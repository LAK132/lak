if not get_option('lak_enable_windowing')
	warning('x3f-view requires windowing')
	subdir_done()
endif
if not get_option('lak_enable_imgui')
	warning('x3f-view requires imgui')
	subdir_done()
endif

executable(
	'x3f-view',
	[
		'main.cpp',
	],
	dependencies: [
		lak_dep,
		imgui_memory_editor_dep,
	],
	build_by_default: false,
	install: get_option('lak_install_targets').contains('x3f-view'),
	install_rpath: get_option('prefix')/get_option('libdir'),
)
