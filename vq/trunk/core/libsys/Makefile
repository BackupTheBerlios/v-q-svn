BASE=../../

.PHONY: auto-mkdir

all: auto-mkdir libsys.so

auto-mkdir:
	mkdir -p auto

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

cpoll.o: $(BASE)/comp-so cpoll.cc cpoll.hpp
	$(BASE)/comp-so cpoll.cc

dirs.o: $(BASE)/comp-so dirs.cc dirs.hpp
	$(BASE)/comp-so dirs.cc

fd.o: $(BASE)/comp-so fd.cc fd.hpp
	$(BASE)/comp-so fd.cc

fdstr.o: $(BASE)/comp-so fdstr.cc fdstr.hpp fd.hpp
	$(BASE)/comp-so fdstr.cc

getline.o: $(BASE)/comp-so getline.cc getline.hpp fd.hpp fdstr.hpp 
	$(BASE)/comp-so getline.cc

hmac_md5.o: $(BASE)/comp-so hmac_md5.c auto/lmd5.h
	$(BASE)/comp-so hmac_md5.c

lock.o: $(BASE)/comp-so lock.cc lock.hpp auto/hasflock.h
	$(BASE)/comp-so lock.cc

sig.o: $(BASE)/comp-so sig.cc sig.hpp
	$(BASE)/comp-so sig.cc

uniq.o: $(BASE)/comp-so uniq.cc uniq.hpp
	$(BASE)/comp-so uniq.cc

util.o: $(BASE)/comp-so util.cc util.hpp
	$(BASE)/comp-so util.cc

#################

clean:
		rm -rf `cat TARGETS`
