project(
	'lua',
	'c',
	license: 'MIT',
	version: '5.4.4',
)

subdir('src')

meson.override_dependency(
	'lua-@0@'.format(meson.project_version()), lua_lib_dep)
meson.override_dependency('lua', lua_lib_dep)

meson.override_find_program('lua', lua)
meson.override_find_program('luac', luac)
