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
	override_options: override_options,
	dependencies: [
		lak_dep,
	],
)
