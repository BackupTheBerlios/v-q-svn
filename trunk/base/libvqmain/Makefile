BASE=../..
INCS=-I$(BASE)/libmain -I.. `cat $(BASE)/corba.inc`

.PHONY: all clean install

all: .depend libvqmain.a

include $(BASE)/Makefile.common

##################

libvqmain.a: $(BASE)/make-lib vqmain.o 
	$(BASE)/make-lib libvqmain.a vqmain.o

.cc.o: $(BASE)/compile $(BASE)/corba.inc
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

#################
install: all
	$(INSTALL) -D -m 0644 vqmain.hpp $(PREFIX)/include/v-q/base/libvqmain/vqmain.hpp
	$(INSTALL) -D -m 0755 libvqmain.a $(PREFIX)/lib/v-q/libvqmain.a
