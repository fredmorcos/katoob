gnome = import('gnome')

resources_xml = configure_file(
  input: app_id + '.gresource.xml.in',
  output: app_id + '.gresource.xml',
  configuration: conf
)

configure_file(
  input: 'mainWindow.ui.in',
  output: 'mainWindow.ui',
  configuration: conf,
)

resources = gnome.compile_resources(
  'resources',
  resources_xml,
  source_dir: meson.global_build_root() / 'resources',
)
