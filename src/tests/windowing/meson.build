laktestwindowing = static_library(
  'laktestwindowing',
  [
    'basic_program.cpp',
    'window.cpp',
  ],
  override_options: [
    'cpp_std=' + version,
    'warning_level=2',
    'werror=false',
  ],
  include_directories: include_directories('../../..', '../../../inc'),
  link_with: [
    lak,
    lakwindowing,
  ],
  dependencies: sdl2,
)
