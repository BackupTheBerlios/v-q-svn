BASE=..
JADE?=openjade

.PHONY: all clean subdirs


doc-html:
	(cd html/ && env SGML_CATALOG_FILES=/usr/local/share/sgml/catalog \
	openjade -t sgml -i output-html -d ../src/stylesheet.dsl \
	/usr/local/share/sgml/docbook/dsssl/modular/dtds/decls/xml.dcl \
	../src/index.xml ) || exit 1
	cp -f src/arch.en.png html
	cp -f src/GPL.txt html
	cp -f src/stylesheet.css html

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
	rm -rf html/*
