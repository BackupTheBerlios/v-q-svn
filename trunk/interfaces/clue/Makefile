BASE=../..
OBJS=autoresp.o dom_add.o dom_alias_add.o dom_alias_rm.o dom_ip_add.o \
	dom_ip_ls.o dom_ip_rm.o dom_rm.o qt_def_get.o qt_def_set.o \
	qt_get.o qt_set.o udot_add.o udot_ls.o udot_rm.o user_add.o \
	user_auth.o user_pass.o user_rm.o
INCS=-I$(BASE)/core -I$(BASE) -I$(BASE)/libsys -I$(BASE)/libtext


all: .depend $(OBJS)

include $(BASE)/Makefile.common

##################

.cc.o:
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

