subdir('lmdb')

laklmdb = declare_dependency(
  link_with: static_library(
    'laklmdb',
    [
      'lmdb.cpp',
    ],
    override_options: 'cpp_std=' + version,
    include_directories: include_directories('../..', '../../inc'),
    link_with: lak,
    dependencies: lmdb,
  ),
  dependencies: lmdb,
)
