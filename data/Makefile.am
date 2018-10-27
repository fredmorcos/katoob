SUBDIRS = ui
EXTRA_DIST = toolbar

install-data-local:
	$(top_srcdir)/mkinstalldirs $(DESTDIR)$(pkgdatadir)/
	$(INSTALL_DATA) $(srcdir)/toolbar $(DESTDIR)$(pkgdatadir)/toolbar

uninstall-local:
	rm -f $(DESTDIR)$(pkgdatadir)/toolbar
