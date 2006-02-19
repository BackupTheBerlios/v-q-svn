BASE=../..
OBJS=cpgsqlauth.o pgsqlauthd.o cpgsqllog.o pgsqllogd.o
INCS=-I$(BASE)/base -I$(BASE) -I../libvqmain -I$(BASE)/libsys -I$(BASE)/libtext \
	-I$(BASE)/libconf -I$(BASE)/corbautil/cxx/import_export -I$(BASE)/corbautil/cxx/portability \
	-I$(BASE)/corbautil/cxx/PoaUtility \
	`cat $(BASE)/corba.inc` `cat $(BASE)/boost.inc` `pkg-config --cflags libpqxx`
LIBS=-L$(BASE) -L$(BASE)/libmain -lmain \
	-L../libvqmain -lvqmain -L$(BASE)/base -lvq \
	-L$(BASE)/libconf -lconf -L$(BASE)/libsys -lsys -L$(BASE)/libtext -ltext \
	-L$(BASE)/corbautil -lcorbautil \
	`cat $(BASE)/corba.lib` `cat $(BASE)/boost.lib` \
	`cat $(BASE)/boost_thread.lib` `pkg-config --libs libpqxx`

.PHONY: pgsql all clean install install-pgsql

pgsql: .depend pgsqlauthd pgsqllogd

all:

include $(BASE)/Makefile.common

##################
clean:
		rm -rf `cat TARGETS`

pgsqlauthd: $(BASE)/load $(BASE)/corba.lib $(BASE)/boost_thread.lib \
cpgsqlauth.o pgsqlauthd.o
	$(BASE)/load pgsqlauthd cpgsqlauth.o $(LIBS)

pgsqllogd: $(BASE)/load $(BASE)/corba.lib $(BASE)/boost_thread.lib \
cpgsqllog.o pgsqllogd.o
	$(BASE)/load pgsqllogd cpgsqllog.o $(LIBS)

##################

$(OBJS): $(BASE)/compile $(BASE)/corba.inc $(BASE)/boost.inc

.cc.o:
	$(BASE)/compile $(INCS) $<

##################
install:

install-pgsql: pgsql
	$(INSTALL) -d -m 0755 $(PREFIX)/bin
	for p in pgsqllogd pgsqlauthd install.pgsql ; do \
		$(INSTALL) -m 0755 $$p $(PREFIX)/bin/$$p ; \
	done
