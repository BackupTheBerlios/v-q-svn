BASE=../..
OBJS=dom_add.o dom_alias_add.o dom_alias_rm.o dom_ip_add.o \
	dom_ip_ls.o dom_ip_rm.o dom_rm.o qt_def_get.o qt_def_set.o \
	qt_get.o qt_set.o udot_add.o udot_ls.o udot_rm.o user_add.o \
	user_auth.o user_pass.o user_rm.o autoresp.o error2str.o cdom_name2id.o \
	user_pass.o user_conf_add.o user_conf_ls.o user_conf_rm.o
INCS=-I$(BASE)/core -I$(BASE) -I$(BASE)/libsys -I$(BASE)/libtext \
	-I$(BASE)/libvqmain -I$(BASE)/libconf
LIBS=-L$(BASE)/libmain -lmain -L$(BASE)/libvqmain -lvqmain -L. -lcluemain \
	-L$(BASE)/libconf -lconf -L$(BASE)/core -lvq

all: dom_add dom_rm user_add user_rm user_pass user_conf_add user_conf_ls \
user_conf_rm user_auth

include $(BASE)/Makefile.common

##################

libcluemain.a: $(BASE)/make-lib cluemain.o 
	$(BASE)/make-lib libcluemain.a cluemain.o

dom_add: dom_add.o
dom_rm: dom_rm.o

dom_add dom_rm: \
$(BASE)/load $(BASE)/corba.lib error2str.o libcluemain.a
	$(BASE)/load $@ $(LIBS) error2str.o `cat $(BASE)/corba.lib`

user_add: user_add.o
user_rm: user_rm.o
user_pass: user_pass.o
user_conf_add: user_conf_add.o
user_conf_ls: user_conf_ls.o
user_conf_rm: user_conf_rm.o

user_add user_rm user_pass user_conf_add user_conf_ls user_conf_rm: \
$(BASE)/load $(BASE)/corba.lib error2str.o cdom_name2id.o libcluemain.a
	$(BASE)/load $@ $(LIBS) -L$(BASE)/libtext -ltext \
	error2str.o cdom_name2id.o `cat $(BASE)/corba.lib`

user_auth: user_auth.o

user_auth: \
$(BASE)/load $(BASE)/corba.lib error2str.o cdom_name2id.o libcluemain.a
	$(BASE)/load $@ $(LIBS) -L$(BASE)/libtext -ltext -L$(BASE)/libsys -lsys \
	error2str.o cdom_name2id.o `cat $(BASE)/corba.lib`

$(OBJS): $(BASE)/compile

user_conf_add.o user_conf_ls.o user_conf_rm.o: $(BASE)/boost.inc

user_conf_add.o: user_conf_add.cc
	$(BASE)/compile $(INCS) `cat $(BASE)/boost.inc` $<

user_conf_ls.o: user_conf_ls.cc
	$(BASE)/compile $(INCS) `cat $(BASE)/boost.inc` $<

user_conf_rm.o: user_conf_rm.cc
	$(BASE)/compile $(INCS) `cat $(BASE)/boost.inc` $<

.cc.o:
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

