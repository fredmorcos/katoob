i18n = import('i18n')

extra_gettext_args = [
  '--directory=' + meson.global_source_root(),
  '--from-code=UTF-8',
]

i18n.gettext(
  app_name,
  preset: 'glib',
  args: extra_gettext_args,
)
