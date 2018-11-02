SUBDIRS = pixmaps xdgmime
EXTRA_DIST = win32/katoob.rc

bin_PROGRAMS = katoob

AM_CXXFLAGS = $(CFLAGS) $(KATOOB_CFLAGS) $(HIGHLIGHT_CFLAGS) $(GTKSOURCEVIEW_CFLAGS) $(MAEMO_CFLAGS) $(PRINT_CFLAGS)
AM_CFLAGS=$(AM_CXXFLAGS)

katoob_LDADD = $(KATOOB_LIBS) $(HIGHLIGHT_LIBS) $(BZ2_LIBS) $(GZ_LIBS) $(GTKSOURCEVIEW_LIBS) $(MAEMO_LIBS) $(LIBCURL) $(PRINT_LIBS)

katoob_SOURCES = katoob.cc main.cc window.cc document.cc conf.cc statusbar.cc menubar.cc toolbar.cc dialogs.cc encodings.cc mdi.cc utils.cc textbuffer.cc filedialog.cc aboutdialog.cc pipe.cc tempfile.cc searchdialog.cc replacedialog.cc label.cc undoredo.cc dict.cc import.cc export.cc preferencesdialog.cc execdialog.cc openlocationdialog.cc textview.cc applets.cc network.cc

noinst_HEADERS = katoob.hh window.hh document.hh conf.hh statusbar.hh menubar.hh toolbar.hh dialogs.hh encodings.hh mdi.hh utils.hh textbuffer.hh filedialog.hh aboutdialog.hh pipe.hh tempfile.hh searchdialog.hh replacedialog.hh label.hh undoredo.hh dict.hh import.hh export.hh preferencesdialog.hh execdialog.hh spell.hh spelldialog.hh macros.h openlocationdialog.hh textview.hh dbus.hh isocodes.hh spellmenu.hh applets.hh network.hh

if PRINT
katoob_SOURCES += print.cc pagesetup.cc printsettings.cc previewdialog.cc
noinst_HEADERS += print.hh previewdialog.hh printsettings.hh pagesetup.hh
endif

if EMULATOR
katoob_SOURCES += emulator.cc emulator.hh
input_window = inputwindow.hh inputwindow.cc
endif

if MULTIPRESS
katoob_SOURCES += multipresswindow.cc multipress.cc multipress.hh multipresswindow.hh
input_window = inputwindow.hh inputwindow.cc
endif

katoob_SOURCES += $(input_window)

if SPELL
katoob_SOURCES += spell.cc spelldialog.cc spellmenu.cc
AM_CXXFLAGS += $(SPELL_CFLAGS)
katoob_LDADD += $(SPELL_LIBS)
endif

if WIN32
katoob_SOURCES += win32/katoob.rc
endif

if DBUS
katoob_SOURCES += dbus.cc
AM_CXXFLAGS += $(DBUS_CFLAGS)
katoob_LDADD += $(DBUS_LIBS)
endif

if FRIBIDI
katoob_SOURCES += shape_arabic.c shape_arabic.h
AM_CXXFLAGS += $(FRIBIDI_CFLAGS)
katoob_LDADD += $(FRIBIDI_LIBS)
endif

if HIGHLIGHT
katoob_SOURCES += xdgmime/xdgmimealias.c xdgmime/xdgmimeint.h xdgmime/xdgmimeparent.c \
		xdgmime/xdgmimealias.h xdgmime/xdgmimeglob.c xdgmime/xdgmimeparent.h \
		xdgmime/xdgmimeglob.h xdgmime/xdgmimemagic.c xdgmime/xdgmime.c \
		xdgmime/xdgmimemagic.h xdgmime/xdgmimecache.c xdgmime/xdgmime.h \
		xdgmime/xdgmimecache.h xdgmime/xdgmimeint.c
katoob_SOURCES += sourcemanager.cc sourcemanager.hh \
		sourceview.cc sourceview.hh
endif

if ISOCODES
katoob_SOURCES += isocodes.cc
endif

if MAEMO
katoob_SOURCES += maemo-wrapper.cc maemo-wrapper.hh
endif
