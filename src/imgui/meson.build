lakimgui_dependencies = declare_dependency(
	dependencies: [
		lakcore_dep,
		lakwindowing_dep,
		imgui_dep,
		imgui_file_dialog_dep,
		implot_dep,
		implot3d_dep,
		glm_dep,
	],
)

lakimgui = static_library(
	'lakimgui',
	[
		'backend.cpp',
		'cie.cpp',
		'softrender.cpp',
		'widgets.cpp',
	],
	dependencies: lakimgui_dependencies,
)

lakimgui_dep = declare_dependency(
	link_with: lakimgui,
	dependencies: lakimgui_dependencies.partial_dependency(
		includes: true, compile_args: true),
)
meson.override_dependency('lak-imgui', lakimgui_dep)
