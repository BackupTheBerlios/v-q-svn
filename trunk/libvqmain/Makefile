BASE=..
OBJS=vqmain.o
INCS=-I../libmain


all: .depend libvqmain.a

include $(BASE)/Makefile.common

##################

libvqmain.a: $(BASE)/make-lib vqmain.o
	$(BASE)/make-lib libvqmain.a vqmain.o

.cc.o: $(BASE)/compile
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

