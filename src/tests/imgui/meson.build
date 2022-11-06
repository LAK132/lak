laktestimgui = static_library(
	'laktestimgui',
	[
		'imgui.cpp',
	],
	override_options: [
		'cpp_std=' + version,
		'warning_level=2',
		'werror=false',
	],
	dependencies: lakimgui_dep,
)

laktestimgui_dep = declare_dependency(
	link_whole: laktestimgui,
)
