BASE=../..
OBJS=cpgsqlauth.o pgsqlauthd.o
INCS=-I$(BASE)/core -I$(BASE) -I$(BASE)/libsys -I$(BASE)/libtext \
	-I$(BASE)/libconf `cat $(BASE)/corba.inc` `cat pgsql.inc`
LIBS=-Wl,-rpath,`cd $(BASE) && echo \`pwd\`/libsys` \
	-Wl,-rpath,/tmp \
	-Wl,-rpath,`cd $(BASE) && echo \`pwd\`/libtext` \
	-Wl,-rpath,/usr/local/progs/pqxx-gcc2/lib \
	-L$(BASE) -lmain -L$(BASE)/core -lvq \
	-L$(BASE)/libconf -lconf \
	-L$(BASE)/libsys -lsys -L$(BASE)/libtext -ltext \
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

