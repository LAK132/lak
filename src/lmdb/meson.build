laklmdb = static_library(
  'laklmdb',
  [
    'lmdb.cpp',
  ],
  override_options: 'cpp_std=' + version,
  include_directories: [
    include_directories('../..', '../../inc'),
    lmdb_include_directory,
  ],
  link_with: lak,
)
