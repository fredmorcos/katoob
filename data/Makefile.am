SUBDIRS =
EXTRA_DIST = toolbar

if EMULATOR
SUBDIRS +=  emulator
endif

if MULTIPRESS
SUBDIRS +=  multipress
endif

install-data-local:
	$(top_srcdir)/mkinstalldirs $(DESTDIR)$(pkgdatadir)/
	$(INSTALL_DATA) $(srcdir)/toolbar $(DESTDIR)$(pkgdatadir)/toolbar

uninstall-local:
	rm -f $(DESTDIR)$(pkgdatadir)/toolbar
