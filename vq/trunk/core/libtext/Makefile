BASE=../..

.PHONY: auto-mkdir

all: auto-mkdir libtext.so

#######################

auto-mkdir:
	mkdir -p auto

auto/lmd5.c: $(BASE)/compile try/md5.c md5.c1 md5.c2
	( ( cd try && $(BASE)/../compile md5.c ) >/dev/null 2>&1 \
	&& cat md5.c1 || cat md5.c2 ) > auto/lmd5.c
	rm -f try/md5.o 

auto/lmd5.h: $(BASE)/compile try/md5.c md5.h1 md5.h2
	( ( cd try && $(BASE)/../compile md5.c ) >/dev/null 2>&1 \
	&& cat md5.h1 || cat md5.h2 ) > auto/lmd5.h
	rm -f try/md5.o

auto/lmd5.o: $(BASE)/compile auto/lmd5.c auto/lmd5.h
	(cd auto && $(BASE)/../compile lmd5.c)

libtext.so: libtext.so.6.0
	ln -sf libtext.so.6.0 libtext.so

libtext.so.6.0: $(BASE)/make-so lower.o hex.o hmac_md5.o auto/lmd5.o \
split.o
	$(BASE)/make-so libtext.so.6.0 lower.o hex.o hmac_md5.o auto/lmd5.o \
	split.o

hex.o: $(BASE)/comp-so hex.cc hex.hpp
	$(BASE)/comp-so hex.cc

hmac_md5.o: $(BASE)/comp-so hmac_md5.cc auto/lmd5.h hmac_md5.hpp
	$(BASE)/comp-so hmac_md5.cc

lower.o: $(BASE)/comp-so lower.cc lower.hpp 
	$(BASE)/comp-so lower.cc

split.o: $(BAES)/comp-so split.cc split.hpp
	$(BASE)/comp-so split.cc

#####################
clean:
		rm -rf `cat TARGETS`
