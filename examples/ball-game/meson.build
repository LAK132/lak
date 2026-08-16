if not get_option('lak_enable_windowing')
	warning('ball-game requires windowing')
	subdir_done()
endif
if not get_option('lak_renderer').contains('opengl')
	warning('ball-game requires opengl')
	subdir_done()
endif
if not get_option('lak_enable_imgui')
	warning('ball-game requires imgui')
	subdir_done()
endif

executable(
	'ball-game',
	[
		'main.cpp',
		'space.cpp',
	],
	dependencies: [
		lak_dep,
	],
	build_by_default: false,
	install: get_option('lak_install_targets').contains('ball-game'),
	install_rpath: get_option('prefix')/get_option('libdir'),
)
