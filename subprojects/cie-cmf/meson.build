project(
	'cie-cmf',
	'cpp',
	license: 'CC-BY-SA-4.0',
	license_files: 'LICENSE.txt',
)

cie_cmf_dep = declare_dependency(
	include_directories: include_directories('include'),
)

meson.override_dependency(
	'cie-cmf', cie_cmf_dep)
