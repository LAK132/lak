project(
	'implot3d',
	'cpp',
	license: 'MIT',
	license_files: 'LICENSE',
	version: '0.5',
)

includes = include_directories('.')

implot3d = static_library(
	'implot3d',
	[
		'implot3d_demo.cpp',
		'implot3d_items.cpp',
		'implot3d_meshes.cpp',
		'implot3d.cpp',
	],
	override_options: [
		'cpp_std=c++11',
	],
	include_directories: includes,
	dependencies: dependency('imgui'),
)

implot3d_dep = declare_dependency(
	link_with: implot3d,
	include_directories: includes,
	version: meson.project_version(),
)

meson.override_dependency(
	'implot3d-@0@'.format(meson.project_version()),
	implot3d_dep)
meson.override_dependency('implot3d', implot3d_dep)
