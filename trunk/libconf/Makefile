BASE=..
OBJS=cconf.o
INCS=
LIBS=

all: .depend libconf.a

include $(BASE)/Makefile.common

##################

libconf.a: $(BASE)/make-lib cconf.o
	$(BASE)/make-lib libconf.a cconf.o

$(OBJS): $(BASE)/comp-so

.cc.o:
	$(BASE)/comp-so $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

