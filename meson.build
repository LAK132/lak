project(
  'lak',
  ['c', 'cpp'],
  default_options: [
    'warning_level=3',
    'werror=true',
    'b_vscrt=static_from_buildtype',
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
    '-Zc:preprocessor',
    '-DUNICODE',
    '-DWIN32_LEAN_AND_MEAN',
    '-DNOMINMAX',
    '-wd4315',
    '-wd4366',
    '-Wv:18',
  ]

  macro_test_args = project_args + ['-std:' + version]
else
  version = 'c++2a'

  project_args = [
    '-Wfatal-errors',
    '-mavx',
  ]

  macro_test_args = project_args + ['-std=' + version]
endif

project_args += [ '-DSDL_MAIN_HANDLED' ]

cplusplus = cxx.get_define('__cplusplus', args: macro_test_args)

if cxx.compute_int(cplusplus) < 202002
  error('C++ version insufficient, expected 202002L got ' + cplusplus)
endif

add_project_arguments(project_args, language: ['c', 'cpp'])

subdir('inc/SDL')
subdir('src')
subdir('src/opengl')
subdir('src/windowing')
subdir('src/tests')
subdir('src/tests/windowing')

sdl2_dll = configure_file(
  copy: true,
  input: sdl2_dll,
  output: 'SDL2.dll',
)

executable(
  'lak_test',
  [
    'src/tests/test_main.cpp',
  ],
  override_options: 'cpp_std=' + version,
  include_directories: include_directories('inc'),
  link_with: [
    lak,
  ],
  link_whole: [
    laktest,
    laktestwindowing,
  ],
  dependencies: [
    sdl2,
  ],
)
