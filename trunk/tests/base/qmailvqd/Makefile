BASE=../../..
OBJS=ivq.o
INCS=`cat $(BASE)/boost.inc` `cat $(BASE)/corba.inc` -I$(BASE)/libtext \
	-I$(BASE)/libsys -I$(BASE)/corbautil/cxx/import_export \
	-I$(BASE)/base -I$(BASE)/corbautil/cxx/portability
LIBS=`cat $(BASE)/boost.lib` `cat $(BASE)/boost_unit_test.lib` `cat $(BASE)/boost_date_time.lib` \
	`cat $(BASE)/boost_filesystem.lib` \
	-L$(BASE)/corbautil -lcorbautil `cat $(BASE)/corba.lib` \
	-L$(BASE)/base -lvq -L$(BASE)/libtext -ltext -L$(BASE)/libsys -lsys

all: .depend ivq

include $(BASE)/Makefile.common

ivq: $(BASE)/load $(BASE)/boost.lib $(BASE)/boost_unit_test.lib \
$(BASE)/boost_date_time.lib $(BASE)/boost_filesystem.lib \
$(BASE)/corba.lib ivq.o
	$(BASE)/load ivq $(LIBS)

##################

$(OBJS): $(BASE)/compile $(BASE)/boost.inc $(BASE)/corba.inc

.cc.o:
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

#################
tests: all
	./run
