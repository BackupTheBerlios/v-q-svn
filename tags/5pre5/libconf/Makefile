BASE=..
OBJS=conf.o
INCS=
LIBS=

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

