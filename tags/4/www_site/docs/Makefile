BASE=..
FILES=$(wildcard *.xml)
HTML=$(FILES:.xml=.html)

.PHONY: all clean subdirs

all: $(FILES:.xml=.html) subdirs

subdirs:
	cd conf && $(MAKE)

clean:
	rm -rf $(HTML)
	cd conf && $(MAKE) clean
	
%.html: %.xml $(BASE)/xsltproc $(BASE)/xslt/page.xslt
	$(BASE)/xsltproc $(BASE)/xslt/page.xslt '$$base='$(BASE) $< > $@.tmp \
	&& mv -f $@{.tmp,}
	chmod 644 $@
	rm -f $@.tmp
