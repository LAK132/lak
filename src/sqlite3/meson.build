sqlite3 = static_library(
  'sqlite3',
  [
    'sqlite3.c',
    'sqlite3.cpp',
  ],
  override_options: 'cpp_std=' + version,
  include_directories: include_directories('../../inc', '../../inc/sqlite3'),
)

sqlite3_shell = static_library(
  'sqlite3_shell',
  [
    'shell.c',
  ],
  override_options: 'cpp_std=' + version,
  include_directories: include_directories('../../inc', '../../inc/sqlite3'),
  link_with: sqlite3,
)
