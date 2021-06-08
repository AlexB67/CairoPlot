cairoplot_sources = [
  'main.cpp',
  'cairoplot-window.cpp',
]

cairoplot_gtkmmdeps = [
  dependency('gtkmm-4.0', version: '>= 4.0.0'),
]

gnome = import('gnome')

cairoplot_sources += gnome.compile_resources('cairoplot-resources',
  'cairoplot.gresource.xml',
  c_name: 'cairoplot'
)

executable('cairoplot', cairoplot_sources,
  dependencies: [cairoplot_gtkmmdeps, libcairoplot_dep],
  override_options : ['cpp_std=c++17'],
  cpp_args : ['-Wall', '-Wextra', '-pedantic'],
  install: true,
)
