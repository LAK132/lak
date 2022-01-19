subdir('lmdb')

laklmdb = static_library(
  'laklmdb',
  [
    'lmdb.cpp',
  ],
  override_options: 'cpp_std=' + version,
  include_directories: include_directories(
    '../..',
    '../../inc',
    'lmdb/lmdb/libraries/liblmdb',
  ),
  link_with: [
    lak,
    lmdb,
  ],
)
