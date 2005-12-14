BASE=../..

.PHONY: all jdbc

jdbc:

include $(BASE)/Makefile.common

all:

jdbc:
	rm -rf build
	idlj -fall -emitAll -td src -i .. -pkgTranslate vq com.foo_baz.v_q ../auth.idl
	idlj -fall -emitAll -td src -i .. -pkgTranslate vq com.foo_baz.v_q ../logger.idl
	mkdir -p build
	javac -d build -classpath $(BASE)/corbautil/corbautil.jar:src `find src -name "*.java"`
	jar cf jdbcd.jar -C build .

install:

install-jdbc: jdbcd.jar
	for p in jdbcd.jar ; do \
		$(INSTALL) -m 0644 $$p $(PREFIX)/lib/vq/$$p ; \
	done
