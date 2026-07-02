if not get_option('lak_enable_windowing')
	warning('mdc-view requires windowing')
	subdir_done()
endif
if not get_option('lak_enable_imgui')
	warning('mdc-view requires imgui')
	subdir_done()
endif
if not get_option('lak_enable_stb_image_write')
	warning('mdc-view requires stb image write')
	subdir_done()
endif

executable(
	'mdc-view',
	[
		'main.cpp',
	],
	dependencies: [
		lak_dep,
	],
	install: true,
	install_rpath: get_option('prefix')/get_option('bindir'),
)
