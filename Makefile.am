ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src po data m4 maemo
@INTLTOOL_DESKTOP_RULE@

if MAEMO
else
desktopdir  = $(datadir)/applications
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
endif

desktop_in_files = katoob.desktop.in

EXTRA_DIST = TODO INSTALL.in THANKS \
		katoob.desktop.in katoob-icon.png katoob.desktop \
		intltool-extract.in intltool-merge.in intltool-update.in \
		katoob.1 katoob.de.1 Doxyfile.in Doxyfile katoob.desktop.in.in

man_MANS = katoob.1
icondir = $(datadir)/pixmaps
icon_DATA = katoob-icon.png

install-data-local:
	$(top_srcdir)/mkinstalldirs $(DESTDIR)$(mandir)/de/man1/
	$(INSTALL_DATA) $(srcdir)/katoob.de.1 $(DESTDIR)$(mandir)/de/man1/katoob.1

uninstall-local:
	rm -f $(DESTDIR)$(mandir)/de/man1/katoob.1
