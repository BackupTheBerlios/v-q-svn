BASE=..
JADE?=openjade

.PHONY: all clean subdirs


doc-html:
	(cd html/ && env SGML_CATALOG_FILES=/usr/local/share/sgml/catalog \
	openjade -t sgml -d /usr/local/share/sgml/docbook/dsssl/modular/html/docbook.dsl \
	/usr/local/share/sgml/docbook/dsssl/modular/dtds/decls/xml.dcl \
	../src/index.xml )
	cp -f src/arch.en.png html
	cp -f src/GPL.txt html

all: 

clean:
	rm -rf html/*
