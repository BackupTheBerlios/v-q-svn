BASE=..
OBJS=vq.o auth.o logger.o autoresp.o
INCS=`cat $(BASE)/corba.inc`
LIBS=`cat $(BASE)/corba.lib`
SUBDIR=pgsql qmailvqd

all:: .depend libvq.a libautoresp.a
	@for entry in $(SUBDIR); do \
		( \
			echo "===> $$entry" ; \
			cd $$entry ; \
			$(MAKE) $@; \
		); \
	done

include $(BASE)/Makefile.common

clean:
	rm -rf `cat TARGETS`
	@for entry in $(SUBDIR); do \
		( \
			echo "===> $$entry" ; \
			cd $$entry ; \
			$(MAKE) $@; \
		); \
	done

##################

libvq.a: $(BASE)/make-lib auth.o vq.o logger.o
	$(BASE)/make-lib libvq.a auth.o vq.o logger.o

libautoresp.a: $(BASE)/make-lib autoresp.o
	$(BASE)/make-lib libautoresp.a autoresp.o

auth.cc auth.hpp: $(BASE)/idl auth.idl vq.idl vq.hpp
	$(BASE)/idl auth.idl

logger.cc logger.hpp: $(BASE)/idl logger.idl vq.idl vq.hpp
	$(BASE)/idl logger.idl

vq.cc vq.hpp: $(BASE)/idl vq.idl
	$(BASE)/idl vq.idl

$(OBJS): $(BASE)/comp-so $(BASE)/corba.inc

.cc.o:
	$(BASE)/comp-so $(INCS) $<

autoresp.o:
	$(BASE)/comp-so -I$(BASE)/libmain -I$(BASE)/libtext -I$(BASE)/libsys \
	-I$(BASE)/libvqmain $<

