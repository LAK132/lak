if not get_option('lak_enable_windowing')
	warning('tiff-view requires windowing')
	subdir_done()
endif
if not get_option('lak_enable_imgui')
	warning('tiff-view requires imgui')
	subdir_done()
endif

executable(
	'tiff-view',
	[
		'main.cpp',
	],
	dependencies: [
		lak_dep,
	],
	build_by_default: false,
	install: get_option('lak_install_targets').contains('tiff-view'),
	install_rpath: get_option('prefix')/get_option('libdir'),
)
