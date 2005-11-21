BASE=../../..
OBJS=iauth.o
INCS=`cat $(BASE)/boost.inc` `cat $(BASE)/corba.inc` -I$(BASE)/libsys \
	-I$(BASE)/libtext -I$(BASE)/base -I$(BASE)/corbautil/cxx/import_export \
	-I$(BASE)/corbautil/cxx/portability
LIBS=`cat $(BASE)/boost.lib` `cat $(BASE)/boost_unit_test.lib` \
	`cat $(BASE)/boost_date_time.lib` \
	-L$(BASE)/corbautil -lcorbautil `cat $(BASE)/corba.lib` \
	-L$(BASE)/base -lvq -L$(BASE)/libtext -ltext

all: .depend iauth 

include $(BASE)/Makefile.common

iauth: $(BASE)/load $(BASE)/boost.lib $(BASE)/boost_unit_test.lib \
$(BASE)/boost_date_time.lib \
$(BASE)/corba.lib iauth.o
	$(BASE)/load iauth $(LIBS)

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
