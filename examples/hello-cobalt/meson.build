if not get_option('lak_enable_windowing')
	warning('hello-cobalt requires windowing')
	subdir_done()
endif
if not get_option('lak_renderer').contains('cobalt')
	warning('hello-cobalt requires cobalt')
	subdir_done()
endif
if not get_option('lak_enable_imgui')
	warning('hello-cobalt requires imgui')
	subdir_done()
endif

hello_cobalt = files([
	'main.cpp',
])

executable(
	'hello-cobalt',
	hello_cobalt,
	dependencies: [
		lak_dep,
	],
	build_by_default: false,
	install: get_option('lak_install_targets').contains('hello-cobalt'),
	install_rpath: get_option('prefix')/get_option('bindir'),
)
