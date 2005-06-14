BASE=..
LANGS=en pl

.PHONY: all all-doc clean subdirs

all-doc: doc-html 
#doc-rtf

include $(BASE)/Makefile.common

doc-html:
	for l in $(LANGS) ; do \
		./insert_code.pl $$l.src .xml ; \
		sed -e 's#\(<\!--ENV:PREFIX-->\).*\(<\!--/ENV:PREFIX-->\)#\1$(PREFIX)\2#' < $$l.src/install.xml > $$l.src/install.xml.tmp ; \
		mv $$l.src/install.xml.tmp $$l.src/install.xml ; \
		(cd $$l.html/ && env SGML_CATALOG_FILES=$(SGML_CATALOG_FILES) \
		openjade -t sgml -i output-html -d ../$$l.src/stylesheet.dsl \
		$(SGML_XML_DCL) \
		../$$l.src/index.xml ) || exit 1 ; \
		cp -f $$l.src/arch.*.png $$l.html ; \
		cp -f $$l.src/GPL.txt $$l.html ; \
		cp -f $$l.src/stylesheet.css $$l.html ; \
		cp -f common/* $$l.html ; \
		(cd $$l.html && ../mime-type.sh) || exit 1; \
	done

doc-rtf:
	cp -f src/arch.en.png rtf
	cp -f src/GPL.txt rtf
	cp -f src/stylesheet.css rtf
	(cd rtf/ && env SGML_CATALOG_FILES=$(SGML_CATALOG_FILES) \
	openjade -t rtf -i output-print -d ../src/stylesheet.dsl \
	$(SGML_XML_DCL) \
	../src/index.xml ) || exit 1

all: 

clean:
	for l in $(LANGS) ; do \
		rm -rf $$l.html/* ; \
	done
