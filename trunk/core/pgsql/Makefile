BASE=../..
OBJS=cpgsqlauth.o
INCS=-I$(BASE)/core -I$(BASE) -I$(BASE)/libsys -I$(BASE)/libtext \
`cat ../../corba.inc` `cat pgsql.inc`
LIBS=`cat ../../corba.lib` `cat pgsql.lib`

all: ../../corba.lib pgsql.lib .depend $(OBJS)

include $(BASE)/Makefile.common

##################

$(OBJS): ../../corba.inc pgsql.inc

.cc.o:
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

