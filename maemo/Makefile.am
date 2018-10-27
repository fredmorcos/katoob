EXTRA_DIST = katoob.service.in

install-data-local:
if MAEMO
	$(top_srcdir)/mkinstalldirs $(DESTDIR)/usr/share/dbus-1/services/
	$(INSTALL_DATA) $(srcdir)/katoob.service $(DESTDIR)/usr/share/dbus-1/services/katoob.service
endif

uninstall-local:
if MAEMO
	rm -f $(DESTDIR)/usr/share/dbus-1/services/katoob.service
endif
