BASE=..
OBJS=sig.o cpoll.o uniq.o fd.o fdstr.o getline.o \
util.o dirs.o lock.o
INCS=

.PHONY: all clean install

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

#################
install: all
	$(INSTALL) -d -m 0755 $(PREFIX)/include/v-q/libsys
	for p in *.hpp ; do \
		$(INSTALL) -m 0644 $$p $(PREFIX)/include/v-q/libsys/$$p ; \
	done
	$(INSTALL) -d -m 0755 $(PREFIX)/lib/v-q
	$(INSTALL) -m 0755 libsys.so.6.0 $(PREFIX)/lib/v-q/libsys.so.6.0
	$(INSTALL) -m 0755 libsys.so $(PREFIX)/lib/v-q/libsys.so
