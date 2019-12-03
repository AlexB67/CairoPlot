cairoplot_sources = [
  'main.cpp',
  'cairoplot-window.cpp',
  'plotter.cpp',
  'plotterlabels.cpp',
  'plotteroptions.cpp',
  'plotterthemes.cpp',
  'plotterwidgets.cpp',
  'plotterevents.cpp'
]

cairoplot_deps = [
  dependency('gtkmm-3.0', version: '>= 3.18'),
]

gnome = import('gnome')

cairoplot_sources += gnome.compile_resources('cairoplot-resources',
  'cairoplot.gresource.xml',
  c_name: 'cairoplot'
)

executable('cairoplot', cairoplot_sources,
  dependencies: cairoplot_deps,
  override_options : ['cpp_std=c++17'],
  cpp_args : '-Wl, -Wall, -Wextra -Wshadow',
  install: true,
)
