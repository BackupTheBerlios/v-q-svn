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
	$(INSTALL) -d -m 0755 $(PREFIX)/include/v-q/libconf
	$(INSTALL) -d -m 0755 $(PREFIX)/lib/v-q
	$(INSTALL) -m 0644 conf.hpp $(PREFIX)/include/v-q/libconf/conf.hpp
	$(INSTALL) -m 0755 libconf.a $(PREFIX)/lib/v-q/libconf.a
