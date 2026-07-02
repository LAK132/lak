if not get_option('lak_enable_windowing')
	warning('template requires windowing')
	subdir_done()
endif
if not get_option('lak_enable_imgui')
	warning('template requires imgui')
	subdir_done()
endif

executable(
	'template',
	[
		'main.cpp',
	],
	dependencies: [
		lak_dep,
	],
	install_rpath: get_option('prefix')/get_option('bindir'),
)
