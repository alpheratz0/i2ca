.POSIX:
.PHONY: all clean install uninstall dist

include config.mk

all: i2ca

i2ca: i2ca.o
	$(CC) $(LDFLAGS) -o i2ca i2ca.o

clean:
	rm -f i2ca i2ca.o i2ca-$(VERSION).tar.gz

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f i2ca $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/i2ca
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp -f i2ca.1 $(DESTDIR)$(MANPREFIX)/man1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/i2ca.1

dist: clean
	mkdir -p i2ca-$(VERSION)
	cp -R COPYING config.mk Makefile README i2ca.1 i2ca.c \
		vendor i2ca-$(VERSION)
	tar -cf i2ca-$(VERSION).tar i2ca-$(VERSION)
	gzip i2ca-$(VERSION).tar
	rm -rf i2ca-$(VERSION)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/i2ca
	rm -f $(DESTDIR)$(MANPREFIX)/man1/i2ca.1
