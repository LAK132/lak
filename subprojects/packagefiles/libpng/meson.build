project(
	'libpng',
	'cpp',
	license: 'Libpng',
	license_files: 'LICENSE',
	version: '1.6.38',
)

cmake = import('cmake')

# --- zlib ---

zlib_version = '1.2.13'

zlib_opts = cmake.subproject_options()

zlib_opts.add_cmake_defines({
})

zlib_subprj = cmake.subproject(
	'zlib-@0@'.format(zlib_version),
	options: zlib_opts)
# zlib_dep
# zlibstatic_dep
# minigzip_exe

zlib_dep = zlib_subprj.dependency('zlibstatic')

# --- libpng ---

cmake = find_program('cmake')

if get_option('buildtype') == 'debug'
	libpng_build_type = 'Debug'
elif get_option('buildtype') == 'release'
	libpng_build_type = 'Release'
elif get_option('buildtype') == 'debugoptimized'
	libpng_build_type = 'RelWithDebInfo'
elif get_option('buildtype') == 'minsize'
	libpng_build_type = 'MinSizeRel'
else
	error('unsupported build type')
endif

libpng_build = custom_target(
	'configure-libpng',
	input: zlib_subprj.target('zlibstatic'),
	depends: zlib_subprj.target('zlibstatic'),
	output: 'libpng.spec',
	console: true,
	command: [
		cmake,
		'-DPNG_STATIC=ON',
		'-DPNG_EXECUTABLES=OFF',
		'-DPNG_TESTS=OFF',
		'-DPNG_BUILD_ZLIB=ON',
		'-DCMAKE_C_FLAGS=-I@0@ -I@1@'.format(
			meson.global_source_root() / 'subprojects' / 'zlib-@0@'.format(zlib_version),
			meson.global_build_root() / 'subprojects' / 'zlib-@0@'.format(zlib_version) / '__CMake_build'),
		'-DZLIB_LIBRARIES=' + zlib_subprj.target('zlibstatic').full_path(),
		'-DCMAKE_BUILD_TYPE=' + libpng_build_type,
		'-DCMAKE_INSTALL_PREFIX=@CURRENT_SOURCE_DIR@/build',
		'-S', '@CURRENT_SOURCE_DIR@',
		'-B', '@OUTDIR@',
		'-G', 'Ninja',
	],
)

libpng_build_dir = libpng_build[0].full_path()/'..'

if host_machine.system() == 'windows'
	if get_option('buildtype') == 'debug'
		lib_suffix = '_staticd.lib'
	else
		lib_suffix = '_static.lib'
	endif
else
	lib_suffix = '.a'
endif

cmake_generated_libpng_lib = custom_target(
	'compile-libpng',
	depends: libpng_build,
	output: 'libpng16' + lib_suffix,
	console: true,
	command: [
		cmake,
		'--build', libpng_build_dir,
		'--target', 'png_static',
	],
)

pnglibconf_h = custom_target(
	'pnglibconf_h',
	depends: libpng_build,
	output: 'pnglibconf.h',
	console: true,
	command: [find_program('python3'), '-c', ''],
)

libpng = static_library(
	'libpng',
	[
		cmake_generated_libpng_lib,
	],
)

libpng_inc = include_directories('.')

libpng_dep = declare_dependency(
	link_with: libpng,
	include_directories: libpng_inc,
	dependencies: zlib_dep,
	sources: pnglibconf_h,
	version: meson.project_version(),
)

meson.override_dependency(
	'libpng-@0@'.format(meson.project_version()),
	libpng_dep)
meson.override_dependency('libpng', libpng_dep)
