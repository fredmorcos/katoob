i18n = import('i18n')

desktop_in = configure_file(
  input: app_id.to_lower() + '.desktop.in.in',
  output: app_id.to_lower() + '.desktop.in',
  configuration: conf)

i18n.merge_file(
  input: desktop_in,
  output: app_id.to_lower() + '.desktop',
  type: 'desktop',
  po_dir: meson.global_source_root() / 'po',
  install: true,
  install_dir: prefix / desktop_entries_dir
)
