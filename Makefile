## -----------------------------
# if either of these are changed, do 'make clean' and rebuild
PREFIX = /usr/local
LOCALEDIR = $(PREFIX)/share/locale
## -----------------------------

.PHONY: example-prog install uninstall pot translations clean

example-prog:
	echo '(-DLOCALEDIR=$(LOCALEDIR))' > gettext/config.locale; \
	dune build --profile release; \
	cp -p _build/default/src/main.exe example-prog; \
	chmod 0755 example-prog

install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin; \
	mkdir -p $(DESTDIR)$(LOCALEDIR); \
	cp example-prog $(DESTDIR)$(PREFIX)/bin/example-prog; \
	cp -r locale/* $(DESTDIR)$(LOCALEDIR)/; \
	chmod 0755 $(DESTDIR)$(PREFIX)/bin/example-prog

uninstall:
	rm -f $(PREFIX)/bin/example-prog; \
	cd locale ; for files in *; \
	do rm -f $(LOCALEDIR)/$$files/LC_MESSAGES/example-prog.mo; \
	done

pot:
	xgettext -o po/example-prog.pot gtk/*.c; \
	xgettext -j -L lisp -o po/example-prog.pot src/*.ml

translations:
	(cd po ; ./make_translations)

clean:
	dune clean; \
	rm -f  example-prog gettext/config.locale
