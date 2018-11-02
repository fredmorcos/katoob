project('katoob', ['cpp', 'c'])

extra_args = ['-Wextra', '-Wpedantic']
add_global_arguments(extra_args, language: 'cpp')
add_global_arguments(extra_args, language: 'c')

extra_clang_args = ['-ferror-limit=0', '-Db_lundef=false']

if meson.get_compiler('cpp').get_id() == 'clang'
    add_global_arguments(extra_clang_args, language: 'cpp')
endif

if meson.get_compiler('c').get_id() == 'clang'
    add_global_arguments(extra_clang_args, language: 'c')
endif

isocodes = dependency('iso-codes')
isocodes_prefix = isocodes.get_pkgconfig_variable('prefix')

deps = [dependency('gtkmm-2.4'),
        dependency('glibmm-2.4'),
        dependency('gconfmm-2.6'),
        dependency('gtksourceview-2.0'),
        dependency('enchant-2'),
        dependency('dbus-1'),
        dependency('dbus-glib-1'),
        dependency('expat'),
        dependency('libcurl'),
        isocodes]

conf = configuration_data()
conf.set('package', 'katoob')
conf.set('version', '0.6.0-alpha')
conf.set('prefix', get_option('prefix'))
conf.set('datadir', get_option('datadir'))
conf.set('isocodes_prefix', isocodes_prefix)

configure_file(input: 'config.h.in',
               output: 'config.h',
               configuration: conf)

configure_file(input: 'katoob.desktop.in',
               output: 'katoob.desktop',
               configuration: conf,
               install_dir: join_paths(get_option('datadir'), 'applications'))

conf_inc = include_directories('.')

install_man('man/katoob.1')
install_man('man/katoob.de.1',
            install_dir: join_paths(get_option('mandir'), 'de', 'man1'))

ui_dir = join_paths(get_option('datadir'), 'katoob', 'ui')
install_data('ui/bars.xml', install_dir: ui_dir)
install_data('ui/toolbar',  install_dir: ui_dir)

pixmaps_dir = join_paths(get_option('datadir'), 'katoob', 'pixmaps')
install_data('pixmaps/green.png',        install_dir: pixmaps_dir)
install_data('pixmaps/red.png',          install_dir: pixmaps_dir)
install_data('pixmaps/katoob.png',       install_dir: pixmaps_dir)
install_data('pixmaps/katoob-small.png', install_dir: pixmaps_dir)
install_data('pixmaps/katoob-large.png', install_dir: pixmaps_dir)
install_data('pixmaps/katoob.svg',       install_dir: pixmaps_dir)

docs_dir = join_paths(get_option('datadir'), 'katoob', 'docs')
install_data('LICENSE',   install_dir: docs_dir)
install_data('README.md', install_dir: docs_dir)

executable(
    'katoob',
    ['src/aboutdialog.cc',
     'src/applets.cc',
     'src/bars.cc',
     'src/conf.cc',
     'src/dbus.cc',
     'src/dialogs.cc',
     'src/dict.cc',
     'src/docfactory.cc',
     'src/document.cc',
     'src/encodings.cc',
     'src/execdialog.cc',
     'src/export.cc',
     'src/filedialog.cc',
     'src/import.cc',
     'src/isocodes.cc',
     'src/katoob.cc',
     'src/label.cc',
     # 'src/maemo-wrapper.cc',
     'src/main.cc',
     'src/mdi.cc',
     'src/network.cc',
     'src/openlocationdialog.cc',
     'src/pagesetup.cc',
     'src/pipe.cc',
     'src/preferencesdialog.cc',
     'src/previewdialog.cc',
     'src/print.cc',
     'src/printdialog.cc',
     'src/printsettings.cc',
     'src/recent.cc',
     'src/replacedialog.cc',
     'src/searchdialog.cc',
     'src/sourcemanager.cc',
     'src/sourceview.cc',
     'src/spell.cc',
     'src/spelldialog.cc',
     'src/spellmenu.cc',
     'src/statusbar.cc',
     'src/tempfile.cc',
     'src/textbuffer.cc',
     'src/textview.cc',
     'src/undoredo.cc',
     'src/utils.cc',
     'src/window.cc',
     'src/xdgmime/xdgmimealias.c',
     'src/xdgmime/xdgmimealias.h',
     'src/xdgmime/xdgmime.c',
     'src/xdgmime/xdgmimecache.c',
     'src/xdgmime/xdgmimecache.h',
     'src/xdgmime/xdgmimeglob.c',
     'src/xdgmime/xdgmimeglob.h',
     'src/xdgmime/xdgmime.h',
     'src/xdgmime/xdgmimeint.c',
     'src/xdgmime/xdgmimeint.h',
     'src/xdgmime/xdgmimemagic.c',
     'src/xdgmime/xdgmimemagic.h',
     'src/xdgmime/xdgmimeparent.c',
     'src/xdgmime/xdgmimeparent.h'],
    install: true,
    dependencies: deps,
    include_directories: conf_inc)
