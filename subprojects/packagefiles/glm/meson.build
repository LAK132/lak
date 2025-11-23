project(
  'glm',
  'cpp',
  license: 'MIT or MIT-Happy-Bunny',
  license_files: 'copying.txt',
	version: '1.0.2',
)

glm_dep = declare_dependency(
  include_directories: [
    '.',
  ],
  version: meson.project_version(),
)

meson.override_dependency(
	'glm-@0@'.format(meson.project_version()),
	glm_dep)
meson.override_dependency('glm', glm_dep)
