BASE=../..
FILES=$(wildcard *.xml)
HTML=$(FILES:.xml=.html)

.PHONY: all clean

all: files.xml $(HTML)

files.xml:
	chmod u+x conf2xml.pl
	./conf2xml.pl > files.xml.tmp
	mv files.xml.tmp files.xml
	$(MAKE) files.html
	
clean:
	rm -rf $(HTML) files.xml
	
%.html: %.xml $(BASE)/xsltproc $(BASE)/xslt/page.xslt
	$(BASE)/xsltproc $(BASE)/xslt/page.xslt '$$base='$(BASE) $< > $@.tmp \
	&& mv -f $@{.tmp,}
	chmod 644 $@
	rm -f $@.tmp
