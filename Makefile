.POSIX:
.PHONY: all clean install uninstall dist

include config.mk

all: png2carr

png2carr: png2carr.o
	$(CC) $(LDFLAGS) -o png2carr png2carr.o $(LDLIBS)

clean:
	rm -f png2carr png2carr.o png2carr-$(VERSION).tar.gz

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f png2carr $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/png2carr
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp -f png2carr.1 $(DESTDIR)$(MANPREFIX)/man1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/png2carr.1

dist: clean
	mkdir -p png2carr-$(VERSION)
	cp -R COPYING config.mk Makefile README png2carr.1 png2carr.c png2carr-$(VERSION)
	tar -cf png2carr-$(VERSION).tar png2carr-$(VERSION)
	gzip png2carr-$(VERSION).tar
	rm -rf png2carr-$(VERSION)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/png2carr
	rm -f $(DESTDIR)$(MANPREFIX)/man1/png2carr.1
