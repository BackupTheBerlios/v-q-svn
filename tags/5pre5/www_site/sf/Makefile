BASE=..
FILES=$(wildcard *.xml)
HTML=$(FILES:.xml=.html)

.PHONY: all clean

all: $(FILES:.xml=.html)
	cd tracker && $(MAKE)

clean:
	rm -rf $(HTML)
	cd tracker && $(MAKE) clean
	
%.html: %.xml $(BASE)/xsltproc $(BASE)/xslt/page.xslt
	$(BASE)/xsltproc $(BASE)/xslt/page.xslt '$$base='$(BASE) $< > $@.tmp \
	&& mv -f $@{.tmp,}
	chmod 644 $@
	rm -f $@.tmp
