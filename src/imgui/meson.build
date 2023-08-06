lakimgui_dependencies = declare_dependency(
	dependencies: [
		lakcore_dep,
		lakwindowing_dep,
		imgui_dep,
		subproject('imgui').get_variable('im_file_dialog_dep'),
		subproject('glm').get_variable('glm_dep'),
	],
)

lakimgui = static_library(
	'lakimgui',
	[
		'backend.cpp',
		'widgets.cpp',
	],
	override_options: 'cpp_std=' + version,
	dependencies: lakimgui_dependencies,
)

lakimgui_dep = declare_dependency(
	link_with: lakimgui,
	dependencies: lakimgui_dependencies.partial_dependency(
		includes: true, compile_args: true),
)
