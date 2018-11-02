EXTRA_DIST = arabic hebrew fr_transliteration
install-data-local:
	$(top_srcdir)/mkinstalldirs $(DESTDIR)$(pkgdatadir)/emulator
	$(INSTALL_DATA) $(srcdir)/arabic $(DESTDIR)$(pkgdatadir)/emulator/arabic
	$(INSTALL_DATA) $(srcdir)/hebrew $(DESTDIR)$(pkgdatadir)/emulator/hebrew
	$(INSTALL_DATA) $(srcdir)/fr_transliteration $(DESTDIR)$(pkgdatadir)/emulator/fr_transliteration

uninstall-local:
	rm -f $(DESTDIR)$(pkgdatadir)/emulator/arabic
	rm -f $(DESTDIR)$(pkgdatadir)/emulator/hebrew
	rm -f $(DESTDIR)$(pkgdatadir)/emulator/fr_transliteration

# FIXME: THIS DOESN'T WORK :(
#	for i in ${EXTRA_DIST};\
#	  do $(INSTALL_DATA) $$i $(DESTDIR)$(pkgdatadir)/emulator/$$i; done
