all: xsltproc index.xml menu.xslt head.html tail.html \
index.html tail.html makefile.xslt DOWNLOAD.html
	./xsltproc makefile.xslt index.xml > Makefile.files.tmp \
	&& mv -f Makefile.files{.tmp,}
	make -f Makefile.files

head.html: head.inc xsltproc menu.xslt index.xml glue.inc
	./xsltproc menu.xslt index.xml > head.tmp
	cat head.inc head.tmp glue.inc > head.html.tmp
	mv -f head.html{.tmp,}
	rm -rf head.tmp

index.html: head.html tail.html xsltproc index.xslt index.xml
	./xsltproc index.xslt index.xml > index.tmp
	cat head.html index.tmp tail.html > index.html.tmp \
	&& mv -f index.html.tmp index.html
	chmod 644 index.html
	rm -f index.tmp

tail.html: tail.inc
	cp -f tail.inc tail.html

DOWNLOAD.html: DOWNLOAD.inc head.html tail.html
	cat head.html DOWNLOAD.inc tail.html > DOWNLOAD.tmp \
	&& mv -f DOWNLOAD.tmp DOWNLOAD.html
	chmod 644 DOWNLOAD.html
	rm -f DOWNLOAD.tmp
	
clean:
	rm -rf `cat TARGETS`

