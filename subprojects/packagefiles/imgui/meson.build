project(
	'imgui',
	'cpp',
	license: 'MIT',
	license_files: 'LICENSE.txt',
	version: '1.92.5',
)

includes = include_directories(
	'.',
	'misc'/'cpp',
)

imgui = static_library(
	'imgui',
	[
		'misc'/'cpp'/'imgui_stdlib.cpp',
		'imgui_demo.cpp',
		'imgui_draw.cpp',
		'imgui_tables.cpp',
		'imgui_widgets.cpp',
		'imgui.cpp',
	],
	override_options: [
		'cpp_std=c++11',
	],
	include_directories: includes,
)

imgui_dep = declare_dependency(
	link_with: imgui,
	include_directories: includes,
	version: meson.project_version(),
)

meson.override_dependency(
	'imgui-@0@'.format(meson.project_version()),
	imgui_dep)
meson.override_dependency('imgui', imgui_dep)
