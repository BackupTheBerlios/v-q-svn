BASE=..

.PHONY: auto-mkdir

all: auto-mkdir .depend libsys.so

auto-mkdir:
	mkdir -p auto

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

cpoll.o: $(BASE)/comp-so cpoll.cc
	$(BASE)/comp-so cpoll.cc

dirs.o: $(BASE)/comp-so dirs.cc
	$(BASE)/comp-so dirs.cc

fd.o: $(BASE)/comp-so fd.cc
	$(BASE)/comp-so fd.cc

fdstr.o: $(BASE)/comp-so fdstr.cc
	$(BASE)/comp-so fdstr.cc

getline.o: $(BASE)/comp-so getline.cc
	$(BASE)/comp-so getline.cc

hmac_md5.o: $(BASE)/comp-so hmac_md5.c
	$(BASE)/comp-so hmac_md5.c

lock.o: $(BASE)/comp-so lock.cc
	$(BASE)/comp-so lock.cc

sig.o: $(BASE)/comp-so sig.cc
	$(BASE)/comp-so sig.cc

uniq.o: $(BASE)/comp-so uniq.cc
	$(BASE)/comp-so uniq.cc

util.o: $(BASE)/comp-so util.cc
	$(BASE)/comp-so util.cc

#################
clean:
		rm -rf `cat TARGETS`
