BASE=../..
OBJS=qmail_files.o qmailvqd.o cqmailvq_dom.o cqmailvq.o cqmailvq_user.o
INCS=-I$(BASE)/core -I$(BASE) -I$(BASE)/libmain -I$(BASE)/libsys -I$(BASE)/libtext \
	-I$(BASE)/libconf `cat $(BASE)/corba.inc`
LIBS=-Wl,-rpath,`cd $(BASE) && echo \`pwd\`/libsys` \
	-Wl,-rpath,/tmp \
	-Wl,-rpath,`cd $(BASE) && echo \`pwd\`/libtext` \
	-Wl,-rpath,/usr/local/progs/pqxx-gcc2/lib \
	-L$(BASE) -L$(BASE)/libmain -lmain -L$(BASE)/core -lvq \
	-L$(BASE)/libconf -lconf \
	-L$(BASE)/libsys -lsys -L$(BASE)/libtext -ltext \
`cat $(BASE)/corba.lib`

all: .auto auto/d_namlen.h .depend qmailvqd

include $(BASE)/Makefile.common

qmailvqd: $(BASE)/load $(BASE)/corba.lib $(OBJS)
	$(BASE)/load qmailvqd $(OBJS) $(LIBS)

auto/d_namlen.h: $(BASE)/compile try/d_namlen.cc d_namlen.h1 d_namlen.h2
	( ( cd try && $(BASE)/compile d_namlen.cc ) >/dev/null 2>&1 \
	&& cat d_namlen.h1 || cat d_namlen.h2 ) > auto/d_namlen.h
	rm -f try/d_namlen.o

##################

$(OBJS): $(BASE)/compile $(BASE)/corba.inc

cqmailvq_user.o: auto/d_namlen.h

.cc.o:
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

