BASE=.
FILES=$(wildcard *.xml)
HTML=$(FILES:.xml=.html)

.PHONY: all clean subdirs

all: $(HTML) subdirs

subdirs:
	cd xslt && $(MAKE)
	cd docs && $(MAKE)
	cd dev && $(MAKE)
	cd sf && $(MAKE)

clean:
	cd docs && $(MAKE) clean
	cd dev && $(MAKE) clean
	cd sf && $(MAKE) clean
	rm -rf *.html
	
%.html: %.xml $(BASE)/xsltproc $(BASE)/xslt/page.xslt
	$(BASE)/xsltproc $(BASE)/xslt/page.xslt $< '$$base='$(BASE) > $@.tmp \
	&& mv -f $@{.tmp,}
	chmod 644 $@
	rm -f $@.tmp
