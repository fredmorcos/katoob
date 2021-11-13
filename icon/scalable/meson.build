install_data(
  app_id + '.svg',
  install_dir: prefix / datadir / 'icons/hicolor/scalable/apps')

gnome = import('gnome')

gnome.post_install(gtk_update_icon_cache: true)
