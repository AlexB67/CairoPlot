project('libcairoplot',  'cpp', 
          version: '0.0.5',
    meson_version: '>= 0.50.0',
  default_options: [ 'warning_level=2',
                     'cpp_std=c++17',
                   ],
)


gtkmmdeps = [
  dependency('gtkmm-3.0', version: '>= 3.18'),
]

inc = include_directories('.')
libcairoplot = static_library('cairoplot',
    'plotter.cpp',
    'plotterlabels.cpp',
    'plotteroptions.cpp',
    'plotterthemes.cpp',
    'plotterwidgets.cpp',
    'plotterevents.cpp',
  dependencies: gtkmmdeps,
  include_directories : inc,
  install : true)

libcairoplot_dep = declare_dependency(include_directories : inc,
  link_with : libcairoplot)