BASE=..
OBJS=conf.o
INCS=
LIBS=

.PHONY: all clean install

all: .depend libconf.a

include $(BASE)/Makefile.common

##################

libconf.a: $(BASE)/make-lib conf.o
	$(BASE)/make-lib libconf.a conf.o

$(OBJS): $(BASE)/comp-so

.cc.o:
	$(BASE)/comp-so $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

#################
install: all
	$(INSTALL) -d -m 0755 $(PREFIX)/include/vq/libconf
	$(INSTALL) -d -m 0755 $(PREFIX)/lib/vq
	$(INSTALL) -m 0644 conf.hpp $(PREFIX)/include/vq/libconf/conf.hpp
	$(INSTALL) -m 0755 libconf.a $(PREFIX)/lib/vq/libconf.a
