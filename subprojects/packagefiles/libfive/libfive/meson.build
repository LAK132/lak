
cmake = import('cmake')

# --- eigen ---

eigen_opts = cmake.subproject_options()

eigen_opts.add_cmake_defines({
	'EIGEN_TEST_CXX11': 'OFF',
	'EIGEN_BUILD_BTL': 'OFF',
	'EIGEN_BUILD_PKGCONFIG': 'OFF',
	'EIGEN_BUILD_DOC': 'OFF',
	'BUILD_TESTING': 'OFF',
	'CMAKE_Fortran_COMPILER': 'NOTFOUND',
})

if meson.get_compiler('cpp').has_argument('bigobj')
	eigen_opts.append_compile_args('cpp', '-bigobj')
endif

eigen_subprj = cmake.subproject('eigen', options: eigen_opts)

eigen_dep = [
	# eigen_subprj.dependency('eigen'),
	eigen_subprj.dependency('eigen_blas_static'),
]

# --- zlib ---

zlib_opts = cmake.subproject_options()

zlib_opts.add_cmake_defines({
})

zlib_subprj = cmake.subproject('zlib', options: zlib_opts)
# zlib_dep
# zlibstatic_dep
# minigzip_exe

zlib_dep = zlib_subprj.dependency('zlibstatic')

# --- libpng ---

libpng_opts = cmake.subproject_options()

libpng_opts.add_cmake_defines({
	'PNG_SHARED': 'OFF',
	'PNG_STATIC': 'ON',
	'PNG_EXECUTABLES': 'OFF',
	'PNG_TEST': 'OFF',
	'PNG_BUILD_ZLIB': 'ON',
})

libpng_opts.append_compile_args(
	'c',
	'-I'+(meson.source_root()/'subprojects/zlib'),
	'-I'+(meson.build_root()/'subprojects/zlib/__CMake_build'),
)

libpng_subprj = cmake.subproject('libpng', options: libpng_opts)

# force zlib to be built at compile stage, while libpng is built at link stage
libpng_dep = declare_dependency(
	dependencies: libpng_subprj.dependency('png_static'),
	sources: custom_target(
		'zlib-libpng-chain-dep',
		output: 'zlib-libpng-chain-dep.hpp',
		input: zlib_subprj.target('zlibstatic'),
		capture: true,
		command: [find_program('python'), '-c', 'print("")'],
	),
)

# --- boost ---

boost_opts = cmake.subproject_options()

boost_opts.add_cmake_defines({
	'BOOST_LOG_WITHOUT_EVENT_LOG': 'ON'
})

if meson.get_compiler('cpp').has_argument('bigobj')
	boost_opts.append_compile_args('cpp', '-bigobj')
endif

boost_subprj = cmake.subproject('boost', options: boost_opts)

boost_dep = [
	boost_subprj.dependency('boost_bimap'),
	boost_subprj.dependency('boost_functional'),
	boost_subprj.dependency('boost_numeric_interval'),
	boost_subprj.dependency('boost_lockfree'),
	boost_subprj.dependency('boost_container'),
]

# --- libfive ---

subdir('src')

includes = include_directories('include')

if meson.get_compiler('cpp').has_argument('bigobj')
	bigobj = '-bigobj'
else
	bigobj = []
endif

libfive_dependencies = declare_dependency(
	dependencies: [
		eigen_dep,
		zlib_dep,
		libpng_dep,
		boost_dep,
	],
)

libfive = static_library(
	'libfive',
	libfive_src,
	cpp_args: [
		'-D_USE_MATH_DEFINES',
		bigobj,
	],
	override_options: [
		'cpp_std=c++17',
		'warning_level=0',
	],
	dependencies: libfive_dependencies,
	include_directories: includes,
)

libfive_dep = declare_dependency(
	link_with: libfive,
	compile_args: '-D_USE_MATH_DEFINES',
	include_directories: includes,
	dependencies: libfive_dependencies.partial_dependency(includes: true),
)

meson.override_dependency('libfive', libfive_dep)
