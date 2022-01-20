subdir('llvm')

lakllvm = declare_dependency(
  link_with: static_library(
    'laktestllvm',
    [
      'llvm.cpp',
    ],
    override_options: [
      'cpp_std=' + version,
      'warning_level=0',
      'werror=false',
    ],
    include_directories: include_directories('../..', '../../inc'),
    link_with: lak,
    dependencies: llvm,
  ),
  dependencies: llvm,
)
