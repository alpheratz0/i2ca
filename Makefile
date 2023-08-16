.POSIX:
.PHONY: all clean install uninstall dist

include config.mk

all: img2carr

img2carr: img2carr.o
	$(CC) $(LDFLAGS) -o img2carr img2carr.o $(LDLIBS)

clean:
	rm -f img2carr img2carr.o img2carr-$(VERSION).tar.gz

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f img2carr $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/img2carr
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp -f img2carr.1 $(DESTDIR)$(MANPREFIX)/man1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/img2carr.1

dist: clean
	mkdir -p img2carr-$(VERSION)
	cp -R COPYING config.mk Makefile README img2carr.1 img2carr.c \
		vendor img2carr-$(VERSION)
	tar -cf img2carr-$(VERSION).tar img2carr-$(VERSION)
	gzip img2carr-$(VERSION).tar
	rm -rf img2carr-$(VERSION)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/img2carr
	rm -f $(DESTDIR)$(MANPREFIX)/man1/img2carr.1
