BASE=../..
OBJS=cpgsqlauth.o pgsqlauthd.o
INCS=-I$(BASE)/core -I$(BASE) -I$(BASE)/libsys -I$(BASE)/libtext \
`cat $(BASE)/corba.inc` `cat pgsql.inc`
LIBS=-L$(BASE)/core -L$(BASE)/libsys -L$(BASE)/libtext \
`cat $(BASE)/corba.lib` `cat pgsql.lib`

all: .depend pgsqlauthd

include $(BASE)/Makefile.common

pgsqlauthd: $(BASE)/load $(BASE)/corba.lib cpgsqlauth.o pgsqlauthd.o
	$(BASE)/load pgsqlauthd cpgsqlauth.o $(LIBS)

##################

$(OBJS): $(BASE)/compile $(BASE)/corba.inc pgsql.inc

.cc.o:
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

