BASE=../..
OBJS=cqmailvq.o
INCS=-I$(BASE)/core -I$(BASE) -I$(BASE)/libsys -I$(BASE)/libtext \
`cat ../../corba.inc`
LIBS=`cat ../../corba.lib`

all: ../../corba.inc .depend $(OBJS)

include $(BASE)/Makefile.common

##################

.cc.o:
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

