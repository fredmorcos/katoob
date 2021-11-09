project(
  'Katoob',
  ['cpp'],
  version: '0.7',
  license: 'GPL2+',
  meson_version: '>= 0.58',
  default_options: [
    'buildtype=debugoptimized',
    'warning_level=3',
    'cpp_std=c++17'
  ]
)

if meson.get_compiler('cpp').get_id() == 'clang'
  extra_flags_clang = [
    '-ferror-limit=0',
    '-Db_lundef=false',
  ]

  add_global_arguments(extra_flags_clang, language: 'cpp')
endif

if target_machine.system() == 'windows'
  windows_flags = [
    '-mms-bitfields',
    '-mwindows',
  ]

  add_global_arguments(windows_flags, language: 'cpp')


  windows_link_flags = [
    '-lwsock32',
  ]

  add_global_link_arguments(windows_link_flags, language: 'cpp')
endif

# Provided by meson.
prefix = get_option('prefix')
bindir = get_option('bindir')
datadir = get_option('datadir')
localedir = get_option('localedir')
localstatedir = get_option('localstatedir')
sysconfdir = get_option('sysconfdir')

# Project-specific.
project = meson.project_name()
executable = project.to_lower()

project_org = 'fredmorcos'
project_org_id = 'com.' + project_org
project_org_path = '/com/' + project_org

project_comment = 'A lightweight, multilingual and bidirectional-aware unicode text editor written in C++ using Gtkmm'
project_url = 'https://github.com/' + project_org + '/' + executable
project_url_label = project + ' on Github'
project_url_issues = project_url + '/issues'

app_id = project_org_id + '.' + project
app_bus_path = project_org_path + '/' + project

app_datadir = datadir / executable
app_docdir = datadir / 'doc' / executable

app_multipress_dir = app_datadir / 'multipress'
app_emulator_dir = app_datadir / 'emulator'

desktop_entries_dir = datadir / 'applications'

message('App ID:', app_id)
message('App (DBus) path:', app_bus_path)
message('App datadir:', app_datadir)
message('App docdir:', app_docdir)
message('App multipress dir:', app_multipress_dir)
message('App emulator dir:', app_emulator_dir)
message('Desktop entries dir:', desktop_entries_dir)

conf = configuration_data()

conf.set('PROJECT_RAW', project, description: 'Project name')
conf.set_quoted('PROJECT', project, description: 'Project name')

conf.set('EXECUTABLE_RAW', executable, description: 'Application, binary or package name')
conf.set_quoted('EXECUTABLE', executable, description: 'Application, binary or package name')

conf.set('VERSION_RAW', meson.project_version(), description: 'Project and package version')
conf.set_quoted('VERSION', meson.project_version(), description: 'Project and package version')

conf.set_quoted('VCS_TAG', '@VCS_TAG@', description: 'Git commit hash')

conf.set('APP_ID_RAW', app_id, description: 'Application ID')
conf.set_quoted('APP_ID', app_id, description: 'Application ID')

conf.set_quoted('APP_BUS_PATH', app_bus_path, description: 'DBus application path')

conf.set('PROJECT_COMMENT_RAW', project_comment, description: 'Project comment')
conf.set_quoted('PROJECT_COMMENT', project_comment, description: 'Project comment')

conf.set('PROJECT_URL_RAW', project_url, description: 'Project website')
conf.set_quoted('PROJECT_URL', project_url, description: 'Project website')

conf.set_quoted('PROJECT_URL_LABEL', project_url_label, description: 'Project website title')

conf.set('PROJECT_URL_ISSUES_RAW', project_url_issues, description: 'Project issues database')
conf.set_quoted('PROJECT_URL_ISSUES', project_url_issues, description: 'Project issues database')

conf.set_quoted('MULTIPRESS_DIR', prefix / app_multipress_dir, description: 'Multipress config file dir')
conf.set_quoted('EMULATOR_DIR', prefix / app_emulator_dir, description: 'Emulator config file dir')

conf.set('APP_DATADIR_RAW', prefix / app_datadir, description: 'Dir for application data files')
conf.set_quoted('APP_DATADIR', prefix / app_datadir, description: 'Dir for application data files')

conf.set('BINDIR_RAW', prefix / bindir, description: 'Dir where binaries are going to be installed')

conf.set_quoted('GETTEXT_PACKAGE', executable, description: 'Gettext package name')
conf.set_quoted('LOCALEDIR', prefix / localedir, description: 'Dir for locale files')

dependencies = [
  dependency('gtkmm-4.0'),
]

gtk_flags  = [
  '-DGTK_DISABLE_DEPRECATED',
  '-DGTK_DISABLE_SINGLE_INCLUDES',
  '-DGDK_DISABLE_DEPRECATED',
  '-DGSEAL_ENABLE',
  '-DGTKMM_DISABLE_DEPRECATED',
  '-DGDKMM_DISABLE_DEPRECATED',
  '-DGLIBMM_DISABLE_DEPRECATED',
  '-DGIOMM_DISABLE_DEPRECATED',
]

add_global_arguments(gtk_flags, language: 'cpp')

enable_bzip2 = get_option('bzip2')

if enable_bzip2
  dependencies += dependency('bzip2')
  conf.set('ENABLE_BZIP2', enable_bzip2, description: 'Bzip2 support is enabled')
endif

enable_dbus = get_option('dbus')

if enable_dbus
  dependencies += dependency('dbus-1')
  dependencies += dependency('dbus-glib-1')
  conf.set('ENABLE_DBUS', enable_dbus, description: 'DBus support is enabled')
endif

enable_dict = get_option('dict')

if enable_dict
  dep_libcurl = dependency('libcurl')
  dep_libcurl_protocols = dep_libcurl.get_variable(pkgconfig: 'supported_protocols')
  dep_libcurl_has_dict = dep_libcurl_protocols.to_lower().contains('dict')

  if dep_libcurl_has_dict == false
    error('Cannot enable DICT: libcurl does not have DICT protocol support built-in')
  endif

  dependencies += dep_libcurl
  conf.set('ENABLE_DICT', enable_dict, description: 'DICT protocol support is enabled')
endif

enable_emulator = get_option('emulator')

if enable_emulator
  conf.set('ENABLE_EMULATOR', enable_emulator, description: 'Keyboard layout emulator is enabled')
endif

enable_fribidi = get_option('fribidi')

if enable_fribidi
  dependencies += dependency('fribidi')
  conf.set('ENABLE_FRIBIDI', enable_fribidi, description: 'Fribidi support is enabled')
endif

enable_gzip = get_option('gzip')

if enable_gzip
  dependencies += dependency('zlib')
  conf.set('ENABLE_GZIP', enable_gzip, description: 'GZip support is enabled')
endif

enable_highlight = get_option('highlight')

if enable_highlight
  dependencies += dependency('shared-mime-info')
  # This completely breaks Gtkmm4 applications. Perhaps switch to gtksourceviewmm-4?
  # dependencies += dependency('gtksourceview-4')
  conf.set('ENABLE_HIGHLIGHT', enable_highlight, description: 'Syntax highlighting is enabled')
endif

enable_spell = get_option('spell')

if enable_spell
  dependencies += dependency('enchant-2')
  conf.set('ENABLE_SPELL', enable_spell, description: 'Spell checking is enabled')
endif

enable_isocodes = get_option('isocodes')

if enable_isocodes
  if enable_spell == false
    error('Cannot enable support for isocodes without spell-checking')
  endif

  dep_isocodes = dependency('iso-codes')
  dep_isocodes_prefix = dep_isocodes.get_variable('prefix')

  dependencies += dep_isocodes
  dependencies += dependency('expat')

  conf.set('ENABLE_ISOCODES', enable_isocodes, description: 'Pretty language names are enabled')
  conf.set_quoted('ISOCODES_PREFIX', dep_isocodes_prefix, description: 'Isocodes installation dir')
endif

enable_multipress = get_option('multipress')

if enable_multipress
  conf.set('ENABLE_MULTIPRESS', enable_multipress, description: 'Multipress is enabled')
endif

enable_nls = get_option('nls')

if enable_nls
  conf.set('ENABLE_NLS', enable_nls, description: 'Native-language support is enabled')
endif

enable_print = get_option('print')

if enable_print
  conf.set('ENABLE_PRINT', enable_print, description: 'Support for printing is enabled')
endif

config_h_in = configure_file(output: 'config.h.in', configuration: conf)
config_h = vcs_tag(
  input: config_h_in,
  output: 'config.h',
  fallback: 'Release',
  command: ['git', 'rev-parse', '--short', 'HEAD']
)

install_data('LICENSE', 'README.md', install_dir: prefix / app_docdir)

subdir('desktop')
subdir('data')
subdir('icon')
subdir('man')
subdir('po')
subdir('src')
