BASE=../..
OBJS=qmailvqd.o cqmailvq_dom.o
INCS=-I$(BASE)/core -I$(BASE) -I$(BASE)/libsys -I$(BASE)/libtext \
	-I$(BASE)/libconf `cat $(BASE)/corba.inc`
LIBS=-Wl,-rpath,`cd $(BASE) && echo \`pwd\`/libsys` \
	-Wl,-rpath,/tmp \
	-Wl,-rpath,`cd $(BASE) && echo \`pwd\`/libtext` \
	-Wl,-rpath,/usr/local/progs/pqxx-gcc2/lib \
	-L$(BASE) -lmain -L$(BASE)/core -lvq \
	-L$(BASE)/libconf -lconf \
	-L$(BASE)/libsys -lsys -L$(BASE)/libtext -ltext \
`cat $(BASE)/corba.lib`

all: .depend qmailvqd

include $(BASE)/Makefile.common

qmailvqd: $(BASE)/load $(BASE)/corba.lib qmailvqd.o cqmailvq_dom.o
	$(BASE)/load qmailvqd qmailvqd.o cqmailvq_dom.o $(LIBS)

##################

$(OBJS): $(BASE)/compile $(BASE)/corba.inc

.cc.o:
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

