BASE=../../..
OBJS=ilogger.o
INCS=`cat $(BASE)/boost.inc` `cat $(BASE)/corba.inc` -I$(BASE)/libsys \
-I$(BASE)/libtext
LIBS=`cat $(BASE)/boost.lib` `cat $(BASE)/corba.lib` -L$(BASE)/core -lvq \
-L$(BASE)/libtext -ltext

all: .depend ilogger 

include $(BASE)/Makefile.common

ilogger: $(BASE)/load $(BASE)/boost.lib $(BASE)/corba.lib ilogger.o
	$(BASE)/load ilogger $(LIBS)

##################

$(OBJS): $(BASE)/compile $(BASE)/boost.inc $(BASE)/corba.inc

.cc.o:
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

