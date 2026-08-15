if not get_option('lak_enable_windowing')
	warning('nbt-view requires windowing')
	subdir_done()
endif
if not get_option('lak_enable_imgui')
	warning('nbt-view requires imgui')
	subdir_done()
endif

executable(
	'nbt-view',
	[
		'main.cpp',
	],
	dependencies: [
		lak_dep,
	],
	build_by_default: false,
	install: get_option('lak_install_targets').contains('nbt-view'),
	install_rpath: get_option('prefix')/get_option('bindir'),
)
