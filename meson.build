project('cbuffer','c')
inc = include_directories('include')

add_project_arguments([
  '-I' + meson.build_root(),
], language: 'c')

subdir('src')
subdir('test')
