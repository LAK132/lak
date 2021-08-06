gl3w = static_library(
  'gl3w',
  'gl3w.c',
  override_options: 'warning_level=2',
  include_directories: include_directories('../..', '../../inc'),
)

lakopengl = static_library(
  'lakopengl',
  [
    'mesh.cpp',
    'shader.cpp',
    'texture.cpp',
  ],
  override_options: 'cpp_std=' + version,
  include_directories: include_directories('../..', '../../inc'),
  dependencies: dependency('threads'),
  link_with: [
    gl3w,
    lak,
  ],
)
