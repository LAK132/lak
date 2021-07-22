project(
  'lak',
  ['c', 'cpp'],
  default_options: [
    'warning_level=2',
    'werror=true',
  ],
)

cxx = meson.get_compiler('cpp')

if cxx.get_id() == 'msvc'
  version = 'c++latest'
  project_args = [
    '-Zc:__cplusplus',
    '-Zc:rvalueCast',
    '-Zc:wchar_t',
    '-Zc:ternary',
    '-DUNICODE',
    '-DWIN32_LEAN_AND_MEAN',
    '-DNOMINMAX',
    '-wd4315',
    '-wd4366',
  ]
  cplusplus = cxx.get_define(
    '__cplusplus',
    args: project_args + ['-std:' + version],
  )
else
  version = 'c++20'
  project_args = [
    '-Wfatal-errors',
  ]
  cplusplus = cxx.get_define(
    '__cplusplus',
    args: project_args + ['-std=' + version],
  )
endif

if cxx.compute_int(cplusplus) < 202002
  error('C++ version insufficient, expected 202002L got ' + cplusplus)
endif

add_project_arguments(
  project_args + [
    # '-DLAK_USE_WINAPI',
    # '-DLAK_USE_XLIB',
    # '-DLAK_USE_XCB',
    '-DLAK_USE_SDL',
  ],
  language: 'cpp'
)

subdir('src')

executable(
  'lak_test',
  [
    'src/tests/test_main.cpp',
  ],
  include_directories: include_directories('inc'),
  override_options: 'cpp_std=' + version,
  dependencies: laktest,
)
