BASE=..
OBJS=mkdepend.o

.PHONY: all clean install

all: mkdepend

include $(BASE)/Makefile.common

mkdepend: $(BASE)/load mkdepend.o
	$(BASE)/load $@

$(OBJS): $(BASE)/compile

.cc.o:
	$(BASE)/compile $(INCS) $<
	
clean:
	rm -rf `cat TARGETS`

install:
