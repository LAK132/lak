gl3w = static_library(
  'gl3w',
  'opengl/gl3w.c',
  override_options: 'warning_level=2',
  include_directories: include_directories('..', '../inc', '../../SDL'),
)

lak = static_library(
  'lak',
  [
    # compression
    'compression/deflate.cpp',
    'compression/lz4.cpp',
    # opengl
    'opengl/mesh.cpp',
    'opengl/shader.cpp',
    'opengl/texture.cpp',
    # general
    'debug.cpp',
    'file.cpp',
    'intrin.cpp',
    'json.cpp',
    'math.cpp',
    'memmanip.cpp',
    'profile.cpp',
    'strconv.cpp',
    'string_view.cpp',
    'test.cpp',
    'tinflate.cpp',
    'tokeniser.cpp',
    'unicode.cpp',
    'unique_pages.cpp',
    # windowing
    # 'events.cpp',
    # 'platform.cpp',
    # 'window.cpp',
  ],
  override_options: 'cpp_std=' + version,
  include_directories: include_directories('..', '../inc', '../../SDL'),
  dependencies: dependency('threads'),
  link_with: gl3w,
)

lakwindowing = static_library(
  'lakwindowing',
  [
    'events.cpp',
    'platform.cpp',
    'window.cpp',
  ],
  override_options: 'cpp_std=' + version,
  include_directories: include_directories('..', '../inc', '../../SDL'),
  link_with: lak,
)

laktest = static_library(
  'laktest',
  [
    'tests/array.cpp',
    'tests/bit_reader.cpp',
    'tests/bitflag.cpp',
    'tests/bitset.cpp',
    'tests/json.cpp',
    'tests/ranges.cpp',
    'tests/test.cpp',
    'tests/trie.cpp',
    'tests/type_pack.cpp',
  ],
  override_options: 'cpp_std=' + version,
  include_directories: include_directories('..', '../inc', '../../SDL'),
  link_with: lak,
)
