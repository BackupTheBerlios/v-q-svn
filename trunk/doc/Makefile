BASE=..
LANGS=en pl
JADE?=openjade

.PHONY: all clean subdirs


doc-html:
	for l in $(LANGS) ; do \
		(cd $$l.html/ && env SGML_CATALOG_FILES=/usr/local/share/sgml/catalog \
		openjade -t sgml -i output-html -d ../$$l.src/stylesheet.dsl \
		/usr/local/share/sgml/docbook/dsssl/modular/dtds/decls/xml.dcl \
		../$$l.src/index.xml ) || exit 1 ; \
		cp -f $$l.src/arch.en.png $$l.html ; \
		cp -f $$l.src/GPL.txt $$l.html ; \
		cp -f $$l.src/stylesheet.css $$l.html ; \
	done

doc-rtf:
	cp -f src/arch.en.png rtf
	cp -f src/GPL.txt rtf
	cp -f src/stylesheet.css rtf
	(cd rtf/ && env SGML_CATALOG_FILES=/usr/local/share/sgml/catalog \
	openjade -t rtf -i output-print -d ../src/stylesheet.dsl \
	/usr/local/share/sgml/docbook/dsssl/modular/dtds/decls/xml.dcl \
	../src/index.xml ) || exit 1

all: 

clean:
	for l in $(LANGS) ; do \
		rm -rf $$l.html/* ; \
	done
