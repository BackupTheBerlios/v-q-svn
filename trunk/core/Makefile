BASE=..
OBJS=auth.o vq.o
INCS=`cat $(BASE)/corba.inc`
LIBS=`cat $(BASE)/corba.lib`

all: .depend libvq.a

include $(BASE)/Makefile.common

##################

libvq.a: $(BASE)/make-lib auth.o vq.o
	$(BASE)/make-lib libvq.a auth.o vq.o

auth.cc auth.hpp: $(BASE)/idl auth.idl vq.idl
	$(BASE)/idl auth.idl

vq.cc vq.hpp: $(BASE)/idl vq.idl
	$(BASE)/idl vq.idl

$(OBJS): $(BASE)/comp-so $(BASE)/corba.inc

.cc.o:
	$(BASE)/comp-so $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

