laktestlmdb = static_library(
  'laktestlmdb',
  [
    'lmdb.cpp',
  ],
  override_options: [
    'cpp_std=' + version,
    'warning_level=0',
    'werror=false',
  ],
  include_directories: include_directories(
    '../../..',
    '../../../inc',
  ),
  link_with: lak,
  dependencies: laklmdb,
)
