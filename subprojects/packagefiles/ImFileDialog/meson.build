project(
	'ImFileDialog',
	'cpp',
	license: 'MIT',
	license_files: 'LICENSE',
)

subproject('stb-f1c79c0')

includes = include_directories('.')

im_file_dialog = static_library(
	'ImFileDialog',
	[
		'ImFileDialog.cpp',
	],
	override_options: [
		'cpp_std=c++17',
	],
	include_directories: includes,
	dependencies: [
		dependency('imgui'),
		dependency('stb_image')
	],
)

im_file_dialog_dep = declare_dependency(
	link_with: im_file_dialog,
	include_directories: includes,
	version: meson.project_version(),
)

meson.override_dependency('ImFileDialog', im_file_dialog_dep)
