BASE=../../..
OBJS=pgsqlauthd.o
INCS=`cat $(BASE)/boost.inc` `cat $(BASE)/corba.inc` -I$(BASE)/libsys \
-I$(BASE)/libtext
LIBS=`cat $(BASE)/boost.lib` `cat $(BASE)/corba.lib` -L$(BASE)/core -lvq \
-L$(BASE)/libtext -ltext

all: .depend pgsqlauthd

include $(BASE)/Makefile.common

pgsqlauthd: $(BASE)/load $(BASE)/boost.lib $(BASE)/corba.lib pgsqlauthd.o
	$(BASE)/load pgsqlauthd $(LIBS)

##################

$(OBJS): $(BASE)/compile $(BASE)/boost.inc $(BASE)/corba.inc

.cc.o:
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

