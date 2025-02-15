laktestimgui = static_library(
	'laktestimgui',
	[
		'imgui.cpp',
	],
	override_options: override_options_w2,
	dependencies: lakimgui_dep,
)

laktestimgui_dep = declare_dependency(
	link_whole: laktestimgui,
)
