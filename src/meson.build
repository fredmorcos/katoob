sources = [
  'aboutdialog.cc',
  'applets.cc',
  'conf.cc',
  'dialogs.cc',
  'document.cc',
  'encodings.cc',
  'execdialog.cc',
  'export.cc',
  'filedialog.cc',
  'import.cc',
  'katoob.cc',
  'label.cc',
  'macros.h',
  'main.cc',
  'mdi.cc',
  'menubar.cc',
  'network.cc',
  'openlocationdialog.cc',
  'pipe.cc',
  'preferencesdialog.cc',
  'redgreen.cc',
  'replacedialog.cc',
  'searchdialog.cc',
  'statusbar.cc',
  'tempfile.cc',
  'textbuffer.cc',
  'textview.cc',
  'toolbar.cc',
  'undoredo.cc',
  'utils.cc',
  'window.cc',
]

if enable_dbus
  sources += 'dbus.cc'
endif

if enable_dict
  sources += 'dict.cc'
endif

if enable_isocodes
  sources += 'isocodes.cc'
endif

if enable_multipress
  sources += 'multipress.cc'
  sources += 'multipresswindow.cc'
  sources += 'inputwindow.cc'
endif

if enable_emulator
  sources += 'emulator.cc'
  sources += 'inputwindow.cc'
endif

if enable_print
  sources += 'pagesetup.cc'
  sources += 'previewdialog.cc'
  sources += 'print.cc'
  sources += 'printsettings.cc'
endif

if enable_highlight
  sources += 'sourcemanager.cc'
  sources += 'sourceview.cc'
endif

if enable_spell
  sources += 'spell.cc'
  sources += 'spelldialog.cc'
  sources += 'spellmenu.cc'
endif

if enable_fribidi
  sources += 'shape_arabic.c'
endif

if target_machine.system() == 'windows'
  sources += 'win32.rc'
endif

# Comes from top-level meson build file.
dependencies += declare_dependency(sources: [config_h])

executable(app_name,
           sources,
           dependencies: dependencies,
           implicit_include_directories: true,
           install: true)
