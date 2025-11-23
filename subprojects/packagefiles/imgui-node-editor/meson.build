project(
	'imgui-node-editor',
	'cpp',
	license: 'MIT',
	license_files: 'LICENSE',
	version: '0.9.2',
)

includes = include_directories('.')

imgui_node_editor = static_library(
	'imgui-node-editor',
	[
		'imgui_canvas.cpp',
		'imgui_node_editor.cpp',
		'imgui_node_editor_api.cpp',
		'crude_json.cpp',
	],
	override_options: [
		'cpp_std=c++14',
	],
	include_directories: includes,
	dependencies: subproject('imgui-1.88').get_variable('imgui_dep'),
)

imgui_node_editor_dep = declare_dependency(
	link_with: imgui_node_editor,
	include_directories: includes,
	version: meson.project_version(),
)

meson.override_dependency(
	'imgui-node-editor-@0@'.format(meson.project_version()),
	imgui_node_editor_dep)
meson.override_dependency('imgui-node-editor', imgui_node_editor_dep)

