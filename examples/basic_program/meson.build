if not get_option('lak_enable_windowing')
	warning('basic_program requires windowing')
	subdir_done()
endif
if not get_option('lak_enable_imgui')
	warning('basic_program requires imgui')
	subdir_done()
endif

executable(
	'basic_program',
	[
		'main.cpp',
	],
	override_options: override_options,
	dependencies: [
		lak_dep,
	],
)
