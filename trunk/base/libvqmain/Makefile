BASE=../..
INCS=-I$(BASE)/libmain -I.. `cat $(BASE)/corba.inc`

all: .depend libvqmain.a

include $(BASE)/Makefile.common

##################

libvqmain.a: $(BASE)/make-lib vqmain.o 
	$(BASE)/make-lib libvqmain.a vqmain.o

.cc.o: $(BASE)/compile $(BASE)/corba.inc
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

