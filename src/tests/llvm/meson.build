laktestllvm = static_library(
  'laktestllvm',
  [
    'llvm.cpp',
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
  dependencies: lakllvm,
)
