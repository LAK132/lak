project(
	'libfive',
	'cpp',
	license: 'MPL-2.0',
	license_files: 'README.md',
)

cmake = import('cmake')

if meson.get_compiler('cpp').has_argument('/bigobj')
	bigobj = '/bigobj'
else
	bigobj = []
endif

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

eigen_opts.append_compile_args('cpp', bigobj)

eigen_subprj = cmake.subproject('eigen-3.4.0', options: eigen_opts)

eigen_dep = [
	# eigen_subprj.dependency('eigen'),
	eigen_subprj.dependency('eigen_blas_static'),
]

# --- boost ---

boost_opts = cmake.subproject_options()

boost_opts.add_cmake_defines({
	'BOOST_LOG_WITHOUT_EVENT_LOG': 'ON',
	'BOOST_INCLUDE_LIBRARIES': 'bimap;functional;numeric/interval;lockfree;container',
})

eigen_opts.append_compile_args('cpp', bigobj)

boost_subprj = cmake.subproject('boost-1.80.0', options: boost_opts)

boost_dep = [
	boost_subprj.dependency('boost_bimap'),
	boost_subprj.dependency('boost_functional'),
	boost_subprj.dependency('boost_numeric_interval'),
	boost_subprj.dependency('boost_lockfree'),
	boost_subprj.dependency('boost_container'),
]

# --- libfive ---

subdir('libfive'/'src')

includes = include_directories('libfive'/'include')

libfive_dependencies = declare_dependency(
	dependencies: [
		eigen_dep,
		boost_dep,
		subproject('libpng-1.6.38').get_variable('libpng_dep'),
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
	version: meson.project_version(),
)

meson.override_dependency('libfive', libfive_dep)
