EXTRA_DIST = bars.xml
install-data-local:
	$(top_srcdir)/mkinstalldirs $(DESTDIR)$(pkgdatadir)/ui
	$(INSTALL_DATA) $(srcdir)/bars.xml $(DESTDIR)$(pkgdatadir)/ui/bars.xml

uninstall-local:
	rm -f $(DESTDIR)$(pkgdatadir)/ui/bars.xml
