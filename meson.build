project('katoob', ['cpp', 'c'],
        license: 'GPL2+',
        version: '0.6.0-alpha',
        meson_version: '>= 0.48.1',
        default_options: ['c_std=c11',
                          'cpp_std=c++17',
                          'warning_level=3'])

extra_args = ['-Wpedantic']
extra_args_clang = ['-ferror-limit=0', '-Db_lundef=false']

add_global_arguments(extra_args, language: 'cpp')
add_global_arguments(extra_args, language: 'c')

if meson.get_compiler('cpp').get_id() == 'clang'
    add_global_arguments(extra_args_clang, language: 'cpp')
endif

if meson.get_compiler('c').get_id() == 'clang'
    add_global_arguments(extra_args_clang, language: 'c')
endif

bzip2 = meson.get_compiler('c').find_library('bz2', required: true)

shared_mime_info = dependency('shared-mime-info')
shared_mime_info_prefix = shared_mime_info.get_pkgconfig_variable('prefix')

isocodes = dependency('iso-codes')
isocodes_prefix = isocodes.get_pkgconfig_variable('prefix')

deps = [dependency('gtkmm-2.4'),
        dependency('glibmm-2.4'),
        dependency('giomm-2.4'),
        dependency('gtksourceview-2.0'),
        dependency('enchant-2'),
        dependency('fribidi'),
        dependency('dbus-1'),
        dependency('dbus-glib-1'),
        dependency('expat'),
        dependency('libcurl'),
        dependency('zlib'),
        bzip2,
        shared_mime_info,
        isocodes]

prefix = get_option('prefix')
datadir = get_option('datadir')
datadir_full = join_paths(prefix, datadir, 'katoob')
mandir = get_option('mandir')
mandir_de = join_paths(mandir, 'de', 'man1')
appsdir = join_paths(datadir, 'applications')
docdir = join_paths(datadir, 'doc', 'katoob')
multipressdir = join_paths(datadir_full, 'multipress')
emulatordir = join_paths(datadir_full, 'emulator')
localedir = join_paths(datadir_full, 'locale')

conf = configuration_data()
conf.set('package', 'katoob')
conf.set('version', meson.project_version())
conf.set('prefix', prefix)
conf.set('datadir', datadir)
conf.set('datadir_full', datadir_full)
conf.set('xdg_prefix', shared_mime_info_prefix)
conf.set('isocodes_prefix', isocodes_prefix)
conf.set('package_locale_dir', localedir)
conf.set('ENABLE_NLS', get_option('enable-nls'))

configure_file(input: 'config.h.in',
               output: 'config.h',
               configuration: conf)

configure_file(input: 'katoob.desktop.in',
               output: 'katoob.desktop',
               configuration: conf,
               install_dir: appsdir)

conf_inc = include_directories('.')

install_man('man/katoob.1')
install_man('man/katoob.de.1', install_dir: mandir_de)

install_data('data/multipress/arabic', install_dir: multipressdir)

install_data('data/emulator/arabic', install_dir: emulatordir)
install_data('data/emulator/hebrew', install_dir: emulatordir)
install_data('data/emulator/fr_transliteration', install_dir: emulatordir)

install_data('data/toolbar', install_dir: datadir_full)

install_data('data/green.png',        install_dir: datadir_full)
install_data('data/red.png',          install_dir: datadir_full)
install_data('data/katoob.png',       install_dir: datadir_full)
install_data('data/katoob-small.png', install_dir: datadir_full)
install_data('data/katoob-large.png', install_dir: datadir_full)
install_data('data/katoob.svg',       install_dir: datadir_full)

install_data('LICENSE',   install_dir: docdir)
install_data('README.md', install_dir: docdir)

executable(
    'katoob',
    ['src/aboutdialog.cc',
     'src/applets.cc',
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
     'src/emulator.cc',
     'src/inputwindow.cc',
     'src/menubar.cc',
     'src/multipress.cc',
     'src/multipresswindow.cc',
     'src/toolbar.cc',
     'src/shape_arabic.c'],
    install: true,
    dependencies: deps,
    include_directories: conf_inc)
