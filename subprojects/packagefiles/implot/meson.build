project(
	'implot',
	'cpp',
	license: 'MIT',
	license_files: 'LICENSE',
	version: '0.17.0',
)

includes = include_directories('.')

implot = static_library(
	'implot',
	[
		'implot_demo.cpp',
		'implot_items.cpp',
		'implot.cpp',
	],
	override_options: [
		'cpp_std=c++11',
	],
	include_directories: includes,
	dependencies: dependency('imgui'),
)

implot_dep = declare_dependency(
	link_with: implot,
	include_directories: includes,
	version: meson.project_version(),
)

meson.override_dependency(
	'implot-@0@'.format(meson.project_version()),
	implot_dep)
meson.override_dependency('implot', implot_dep)
