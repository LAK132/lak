
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

# --- libpng ---

libpng_subprj = subproject('libpng')

libpng_dep = libpng_subprj.get_variable('libpng_dep')

# --- zlib ---

zlib_dep = libpng_subprj.get_variable('zlib_dep')

# --- boost ---

boost_opts = cmake.subproject_options()

boost_opts.add_cmake_defines({
	'BOOST_LOG_WITHOUT_EVENT_LOG': 'ON',
	'BOOST_INCLUDE_LIBRARIES': 'bimap;functional;numeric/interval;lockfree;container',
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
