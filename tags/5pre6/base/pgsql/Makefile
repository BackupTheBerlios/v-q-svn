BASE=../..
OBJS=cpgsqlauth.o pgsqlauthd.o cpgsqllog.o pgsqllogd.o
INCS=-I$(BASE)/base -I$(BASE) -I$(BASE)/libvqmain -I$(BASE)/libsys -I$(BASE)/libtext \
	-I$(BASE)/libconf `cat $(BASE)/corba.inc` `cat $(BASE)/pgsql.inc`
LIBS=-L$(BASE) -L$(BASE)/libmain -lmain \
	-L$(BASE)/libvqmain -lvqmain -L$(BASE)/base -lvq \
	-L$(BASE)/libconf -lconf \
	-L$(BASE)/libsys -lsys -L$(BASE)/libtext -ltext \
	`cat $(BASE)/corba.lib` `cat $(BASE)/pgsql.lib`

pgsql: .depend pgsqlauthd pgsqllogd

all:

include $(BASE)/Makefile.common

clean:
		rm -rf `cat TARGETS`

pgsqlauthd: $(BASE)/load $(BASE)/corba.lib $(BASE)/pgsql.lib cpgsqlauth.o \
pgsqlauthd.o
	$(BASE)/load pgsqlauthd cpgsqlauth.o $(LIBS)

pgsqllogd: $(BASE)/load $(BASE)/corba.lib $(BASE)/pgsql.lib cpgsqllog.o \
pgsqllogd.o
	$(BASE)/load pgsqllogd cpgsqllog.o $(LIBS)

##################

$(OBJS): $(BASE)/compile $(BASE)/corba.inc $(BASE)/pgsql.inc

.cc.o:
	$(BASE)/compile $(INCS) $<
