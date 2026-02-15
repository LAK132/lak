c_args = []
lua_dependencies = []

cc = meson.get_compiler('c')

if cc.get_id() == 'msvc'
	version = 'c99'
	link_args = []
else
	version = 'gnu99'
	link_args = ['-lm']
endif

if host_machine.system() == 'cygwin'
	c_args += [
		# '-DLUA_BUILD_AS_DLL',
	]
elif host_machine.system() == 'windows'
	c_args += [
		# '-DLUA_USE_WINDOWS', # defined automatically by luaconf.h
	]
elif host_machine.system() == 'linux'
	c_args += [
		'-DLUA_USE_LINUX',
	]
	lua_dependencies += [
		dependency('dl'),
	]
	if get_option('lua_linux_use_readline')
		c_args += [
			'-DLUA_USE_READLINE',
		]
		lua_dependencies += [
			dependency('readline'),
		]
	endif
elif host_machine.system() == 'darwin'
	c_args += [
		'-DLUA_USE_MACOSX',
	]
	if get_option('lua_darwin_use_readline')
		c_args += [
			'-DLUA_USE_READLINE',
		]
		lua_dependencies += [
			dependency('readline'),
		]
	endif
elif host_machine.system() == 'sunos'
	c_args += [
		'-DLUA_USE_POSIX',
		'-DLUA_USE_DLOPEN',
		'-D_REENTRANT',
	]
	lua_dependencies += [
		dependency('dl'),
	]
elif host_machine.system() == 'freebsd' or host_machine.system() == 'openbsd' or host_machine.system() == 'netbsd'
	c_args += [
		'-DLUA_USE_LINUX',
		'-DLUA_USE_READLINE',
	]
	lua_dependencies += [
		dependency('edit'),
	]
endif

add_project_arguments(c_args, language: 'c')

lua_export_headers = files(
	'lua.h',
	'luaconf.h',
	'lualib.h',
	'lauxlib.h',
	'lua.hpp',
)

lua_core = static_library(
	'lua-core',
	[
		'lapi.c',
		'lcode.c',
		'lctype.c',
		'ldebug.c',
		'ldo.c',
		'ldump.c',
		'lfunc.c',
		'lgc.c',
		'llex.c',
		'lmem.c',
		'lobject.c',
		'lopcodes.c',
		'lparser.c',
		'lstate.c',
		'lstring.c',
		'ltable.c',
		'ltm.c',
		'lundump.c',
		'lvm.c',
		'lzio.c',
	],
	dependencies: lua_dependencies,
	link_args: link_args,
	override_options: 'c_std=' + version,
)

lua_core_dep = declare_dependency(
	link_with: lua_core,
	include_directories: '.',
	version: meson.project_version(),
)

lua_lib = static_library(
	'lua-lib',
	[
		'lauxlib.c',
		'lbaselib.c',
		'lcorolib.c',
		'ldblib.c',
		'linit.c',
		'liolib.c',
		'lmathlib.c',
		'loadlib.c',
		'loslib.c',
		'lstrlib.c',
		'ltablib.c',
		'lutf8lib.c',
	],
	dependencies: lua_dependencies,
	link_args: link_args,
	override_options: 'c_std=' + version,
)

lua_lib_dep = declare_dependency(
	link_with: lua_lib,
	include_directories: '.',
	dependencies: [lua_core_dep],
	version: meson.project_version(),
)

lua = executable(
	'lua',
	['lua.c'],
	dependencies: [lua_lib_dep],
	link_args: link_args,
)

luac = executable(
	'luac',
	['luac.c'],
	dependencies: [lua_lib_dep],
	link_args: link_args,
)
