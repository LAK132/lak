lakwindowing = static_library(
  'lakwindowing',
  [
    'events.cpp',
    'platform.cpp',
    'window.cpp',
  ],
  override_options: 'cpp_std=' + version,
  include_directories: [
    include_directories([
      '../..',
      '../../../inc',
    ]),
  ],
  link_with: [
    lak,
    lakopengl,
  ],
  dependencies: sdl2,
)
