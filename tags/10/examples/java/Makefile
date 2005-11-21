BASE=../..

.PHONY: all jdbc

all:

include $(BASE)/Makefile.common

all: DomVal.class DomVal1.class DomList.class

%.class: %.java
	$(JAVAC) -classpath $(BASE)/base/jdbc/jdbcd.jar:. $<

install:

clean:
	rm -rf `cat TARGETS`
