BASE=..
OBJS=vq.o auth.o logger.o
INCS=`cat $(BASE)/corba.inc`
LIBS=`cat $(BASE)/corba.lib`
SUBDIR=pgsql qmailvqd

all:: libvq.a
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

libvq.a: $(BASE)/make-lib auth.o vq.o .depend
	$(BASE)/make-lib libvq.a auth.o vq.o

auth.cc auth.hpp: $(BASE)/idl auth.idl vq.idl vq.hpp
	$(BASE)/idl auth.idl

logger.cc logger.hpp: $(BASE)/idl logger.idl vq.idl vq.hpp
	$(BASE)/idl logger.idl

vq.cc vq.hpp: $(BASE)/idl vq.idl
	$(BASE)/idl vq.idl

$(OBJS): $(BASE)/comp-so $(BASE)/corba.inc

.cc.o:
	$(BASE)/comp-so $(INCS) $<

