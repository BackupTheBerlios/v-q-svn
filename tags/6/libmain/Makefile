BASE=..
OBJS=main.o
INCS=

.PHONY: all clean install

all: .depend libmain.a

include $(BASE)/Makefile.common

##################

libmain.a: $(BASE)/make-lib main.o
	$(BASE)/make-lib libmain.a main.o

.cc.o: $(BASE)/compile
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

#################
install: all
	$(INSTALL) -d -m 0755 $(PREFIX)/include/vq/libmain
	$(INSTALL) -m 0644 main.hpp $(PREFIX)/include/vq/libmain/main.hpp
	$(INSTALL) -m 0755 libmain.a $(PREFIX)/lib/vq/libmain.a
