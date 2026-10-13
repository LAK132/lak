if not get_option('lak_enable_windowing')
	warning('colour-demo requires windowing')
	subdir_done()
endif
if not get_option('lak_enable_imgui')
	warning('colour-demo requires imgui')
	subdir_done()
endif

executable(
	'colour-demo',
	[
		'main.cpp',
	],
	dependencies: [
		lak_dep,
		implot_dep,
	],
	build_by_default: false,
	install: get_option('lak_install_targets').contains('colour-demo'),
	install_rpath: get_option('prefix')/get_option('libdir'),
)
