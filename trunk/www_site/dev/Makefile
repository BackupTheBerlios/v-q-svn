BASE=..
FILES=$(wildcard *.xml)
HTML=$(FILES:.xml=.html)

.PHONY: all clean subdirs

all: subdirs history.xml history.html $(FILES:.xml=.html)

subdirs:
	cd source && $(MAKE)

clean:
	rm -rf $(HTML) history.xml
	cd source && $(MAKE) clean
	
%.html: %.xml $(BASE)/xsltproc $(BASE)/xslt/page.xslt
	$(BASE)/xsltproc $(BASE)/xslt/page.xslt '$$base='$(BASE) $< > $@.tmp \
	&& mv -f $@{.tmp,}
	chmod 644 $@
	rm -f $@.tmp

history:
	cm log > history.tmp
	mv history.tmp history

history.xml: history2xml.pl history
	./history2xml.pl > history.xml.tmp
	mv history.xml.tmp history.xml

history.html: history.xml $(BASE)/xsltproc $(BASE)/xslt/page.xslt
	$(BASE)/xsltproc $(BASE)/xslt/page.xslt history.xml '$$base='$(BASE) > $@.tmp \
	&& mv -f $@{.tmp,}
	chmod 644 $@
	rm -f $@.tmp

