BASE=../../..
OBJS=ivq.o
INCS=`cat $(BASE)/boost.inc` `cat $(BASE)/corba.inc` -I$(BASE)/libtext \
	-I$(BASE)/libsys
LIBS=`cat $(BASE)/boost.lib` `cat $(BASE)/boost_unit_test.lib` `cat $(BASE)/boost_date_time.lib` \
	`cat $(BASE)/corba.lib` -L$(BASE)/core -lvq -L$(BASE)/libtext -ltext -L$(BASE)/libsys -lsys

all: .depend ivq

include $(BASE)/Makefile.common

ivq: $(BASE)/load $(BASE)/boost.lib $(BASE)/boost_unit_test.lib $(BASE)/boost_date_time.lib \
$(BASE)/corba.lib ivq.o
	$(BASE)/load ivq $(LIBS)

##################

$(OBJS): $(BASE)/compile $(BASE)/boost.inc $(BASE)/corba.inc

.cc.o:
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

