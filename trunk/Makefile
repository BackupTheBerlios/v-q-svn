BASE=.
OBJS=main.o
INCS=


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

