BASE=..
OBJS=sig.o cpoll.o uniq.o fd.o fdstr.o getline.o \
util.o dirs.o lock.o
INCS=

all: auto-mkdir .depend libsys.so

include $(BASE)/Makefile.common

##################

auto/hasflock.h: try/flock.cc $(BASE)/compile $(BASE)/load
	( ( cd try && $(BASE)/../compile flock.cc && $(BASE)/../load flock ) >/dev/null 2>&1 \
    && echo \#define HASFLOCK 1 || exit 0 ) > auto/hasflock.h
	rm -f try/flock.o try/flock

libsys.so: libsys.so.6.0
	ln -sf libsys.so.6.0 libsys.so

libsys.so.6.0: $(BASE)/make-so sig.o cpoll.o uniq.o fd.o fdstr.o getline.o \
util.o dirs.o lock.o
	$(BASE)/make-so libsys.so.6.0 sig.o cpoll.o uniq.o fd.o fdstr.o getline.o \
	util.o dirs.o lock.o

$(OBJS): $(BASE)/comp-so

lock.o: lock.cc auto/hasflock.h
sig.o: sig.cc
cpoll.o: cpoll.cc
uniq.o: uniq.cc
fd.o: fd.cc
fdstr.o: fdstr.cc
getline.o: getline.cc
util.o: util.cc 
dirs.o: dirs.cc 

.cc.o:
	$(BASE)/comp-so $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`
