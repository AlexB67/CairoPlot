project('libcairoplot',  'cpp', 
          version: '0.2.0',
    meson_version: '>= 0.50.0',
  default_options: [ 'warning_level=2',
                     'cpp_std=c++17',
                   ],
)


gtkmmdeps = [
  dependency('gtkmm-4.0', version: '>= 4.00'),
]

inc = include_directories('.')
libcairoplot = static_library('cairoplot',
    'plotter.cpp',
    'plottercoords.cpp',
    'plotterseries.cpp',
    'plotterlabels.cpp',
    'plotteroptions.cpp',
    'plotterthemes.cpp',
    'plotterwidgets.cpp',
    'plotterevents.cpp',
  dependencies: gtkmmdeps,
  include_directories : inc,
  install : false)

libcairoplot_dep = declare_dependency(include_directories : inc,
  compile_args : ['-Wall', '-Wextra', '-pedantic', '-Wno-overlength-strings'], # shut up gresource
  link_with : libcairoplot)
