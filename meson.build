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
  ]
  cplusplus = cxx.get_define(
    '__cplusplus',
    args: project_args + ['-std:' + version]
  )
else
  version = 'c++20'
  project_args = [
    '-Wfatal-errors',
  ]
  cplusplus = cxx.get_define(
    '__cplusplus',
    args: project_args + ['-std=' + version]
  )
endif

add_project_arguments(
  project_args,
  language: 'cpp'
)

if cxx.compute_int(cplusplus) < 202002
  error('C++ version insufficient, expected 202002L got ' + cplusplus)
endif

add_project_arguments(
  [
    # '-DLAK_USE_WINAPI',
    # '-DLAK_USE_XLIB',
    # '-DLAK_USE_XCB',
    '-DLAK_USE_SDL',
  ],
  language: 'cpp'
)

sources = [
  # compression
  'src/compression/deflate.cpp',
  'src/compression/lz4.cpp',
  # opengl
  'src/opengl/gl3w.c',
  'src/opengl/mesh.cpp',
  'src/opengl/shader.cpp',
  'src/opengl/texture.cpp',
  # tests
  'src/tests/array.cpp',
  'src/tests/bit_reader.cpp',
  'src/tests/bitflag.cpp',
  'src/tests/bitset.cpp',
  'src/tests/test_main.cpp',
  'src/tests/test.cpp',
  'src/tests/trie.cpp',
  'src/tests/type_pack.cpp',
  # general
  'src/debug.cpp',
  'src/file.cpp',
  'src/intrin.cpp',
  'src/memmanip.cpp',
  'src/profile.cpp',
  'src/strconv.cpp',
  'src/string_view.cpp',
  'src/test.cpp',
  'src/tinflate.cpp',
  'src/tokeniser.cpp',
  'src/unicode.cpp',
  'src/unique_pages.cpp',
  # windowing
  # 'src/events.cpp',
  # 'src/platform.cpp',
  # 'src/window.cpp',
]

executable(
  'lak_test',
  sources,
  include_directories: include_directories('inc'),
  override_options: 'cpp_std=' + version,
  dependencies: dependency('threads'),
)
