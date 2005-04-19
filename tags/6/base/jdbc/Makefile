BASE=../..

.PHONY: all jdbc

jdbc:

include $(BASE)/Makefile.common

all:

jdbc:
	rm -rf build source
	if [ -d .svn ] ; then svn export --force . source/ ; else rm -rf source; mkdir -p source ; cp -R *.java com source ; fi
	idlj -fall -emitAll -td source -i .. -pkgTranslate vq com.foo_baz.v_q ../auth.idl
	idlj -fall -emitAll -td source -i .. -pkgTranslate vq com.foo_baz.v_q ../logger.idl
	mkdir -p build
	javac -d build -classpath $(BASE)/corbautil/corbautil.jar:source `find source -name "*.java"`
	jar cf jdbcd.jar -C build .

install:

install-jdbc: jdbcd.jar
	$(INSTALL) -d -m 0755 $(PREFIX)/bin
	for p in jdbcd.jar ; do \
		$(INSTALL) -m 0644 $$p $(PREFIX)/bin/$$p ; \
	done
