laktestlmdb = static_library(
  'laktestlmdb',
  [
    'lmdb.cpp',
  ],
  override_options: [
    'cpp_std=' + version,
    'werror=false',
  ],
  include_directories: include_directories(
    '../../..',
    '../../../inc',
    '../../../..',
    '../../lmdb/lmdb/lmdb/libraries/liblmdb',
  ),
  link_with: [
    lak,
    laklmdb,
  ],
)
