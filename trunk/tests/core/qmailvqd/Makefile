BASE=../../..
OBJS=qmailvqd.o
INCS=`cat $(BASE)/boost.inc` `cat $(BASE)/corba.inc` -I$(BASE)/libtext
LIBS=`cat $(BASE)/boost.lib` `cat $(BASE)/corba.lib` -L$(BASE)/core -lvq \
-L$(BASE)/libtext -ltext

all: .depend qmailvqd

include $(BASE)/Makefile.common

qmailvqd: $(BASE)/load $(BASE)/boost.lib $(BASE)/corba.lib qmailvqd.o
	$(BASE)/load qmailvqd $(LIBS)

##################

$(OBJS): $(BASE)/compile $(BASE)/boost.inc $(BASE)/corba.inc

.cc.o:
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

