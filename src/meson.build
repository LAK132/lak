gl3w = static_library(
  'gl3w',
  'opengl/gl3w.c',
  override_options: 'warning_level=2',
  include_directories: include_directories('..', '../inc'),
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
  ],
  override_options: 'cpp_std=' + version,
  include_directories: include_directories('..', '../inc'),
  dependencies: dependency('threads'),
  link_with: gl3w,
)

laktest = static_library(
  'laktest',
  [
    'tests/array.cpp',
    'tests/bit_reader.cpp',
    'tests/bitflag.cpp',
    'tests/bitset.cpp',
    'tests/json.cpp',
    'tests/test.cpp',
    'tests/trie.cpp',
    'tests/type_pack.cpp',
  ],
  override_options: 'cpp_std=' + version,
  include_directories: include_directories('..', '../inc'),
  link_with: lak,
)
