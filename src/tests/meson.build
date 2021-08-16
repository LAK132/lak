laktest = static_library(
  'laktest',
  [
    'array.cpp',
    'bit_reader.cpp',
    'bitflag.cpp',
    'bitset.cpp',
    'const_string.cpp',
    'dsl.cpp',
    'json.cpp',
    'string_view.cpp',
    'test.cpp',
    'trie.cpp',
    'type_pack.cpp',
  ],
  override_options: 'cpp_std=' + version,
  include_directories: include_directories('../..', '../../inc'),
  link_with: lak,
)
