BASE=../..
OBJS=cpgsqlauth.o pgsqlauthd.o
INCS=-I$(BASE)/core -I$(BASE) -I$(BASE)/libvqmain -I$(BASE)/libsys -I$(BASE)/libtext \
	-I$(BASE)/libconf `cat $(BASE)/corba.inc` `cat pgsql.inc`
LIBS=-L$(BASE) -L$(BASE)/libmain -lmain \
	-L$(BASE)/libvqmain -lvqmain -L$(BASE)/core -lvq \
	-L$(BASE)/libconf -lconf \
	-L$(BASE)/libsys -lsys -L$(BASE)/libtext -ltext \
`cat $(BASE)/corba.lib` `cat pgsql.lib`

pgsql: .depend pgsqlauthd

all::

include $(BASE)/Makefile.common

clean:
		rm -rf `cat TARGETS`

pgsqlauthd: $(BASE)/load $(BASE)/corba.lib cpgsqlauth.o pgsqlauthd.o
	$(BASE)/load pgsqlauthd cpgsqlauth.o $(LIBS)

##################

$(OBJS): $(BASE)/compile $(BASE)/corba.inc pgsql.inc

.cc.o:
	$(BASE)/compile $(INCS) $<
