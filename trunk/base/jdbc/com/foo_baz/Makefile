BASE=../../../..
SUBDIR=v_q

.PHONY: all
.SUFFIXES: .class .java

all:

include $(BASE)/Makefile.common

all: Getopt.class
	@for entry in $(SUBDIR); do \
		( \
			echo "===> $$entry" ; \
			cd $$entry ; \
			$(MAKE) $@; \
		) || exit 1; \
	done

.java.class:
	$(JAVAC) -classpath $(BASE)/corbautil/corbautil.jar:. $<
