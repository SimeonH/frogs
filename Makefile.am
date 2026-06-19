SUBDIRS = src

desktopdir = $(datadir)/applications
desktop_DATA = frogs.desktop

frogs.desktop: frogs.desktop.in
	sed -e 's|@pkgdatadir[@]|$(pkgdatadir)|g' $< > $@

CLEANFILES = frogs.desktop
EXTRA_DIST = bmp wav doc frogs.desktop.in

install-data-local:
	$(MKDIR_P) "$(DESTDIR)$(pkgdatadir)"
	cp -r $(srcdir)/bmp "$(DESTDIR)$(pkgdatadir)/"
	cp -r $(srcdir)/wav "$(DESTDIR)$(pkgdatadir)/"
	cp -r $(srcdir)/doc "$(DESTDIR)$(pkgdatadir)/"

install-data-hook:
	-update-desktop-database "$(DESTDIR)$(desktopdir)" 2>/dev/null || true

uninstall-local:
	rm -rf "$(DESTDIR)$(pkgdatadir)/bmp"
	rm -rf "$(DESTDIR)$(pkgdatadir)/wav"
	rm -rf "$(DESTDIR)$(pkgdatadir)/doc"
	-rm -f "$(DESTDIR)$(desktopdir)/frogs.desktop"
