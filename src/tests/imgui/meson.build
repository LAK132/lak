laktestimgui = static_library(
	'laktestimgui',
	[
		'imgui.cpp',
	],
	override_options: override_options_w2,
	dependencies: [
		lakimgui_dep,
		imgui_node_editor_dep,
		imgui_color_text_edit_dep,
		imgui_memory_editor_dep,
	],
)

laktestimgui_dep = declare_dependency(
	link_whole: laktestimgui,
)
