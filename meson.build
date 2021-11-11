project('cairoplot', ['cpp', 'c'], 
          version: '0.9.0',
    meson_version: '>= 0.50.0',
  default_options: [ 'warning_level=2',
                     'cpp_std=c++17',
                   ],
)

libcairoplot_proj = subproject('libcairoplot')
libcairoplot_dep = libcairoplot_proj.get_variable('libcairoplot_dep')

i18n = import('i18n')

subdir('data')
subdir('src')
subdir('po')

meson.add_install_script('build-aux/meson/postinstall.py')
