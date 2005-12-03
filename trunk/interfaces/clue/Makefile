BASE=../..
OBJS=cluemain.o dom_add.o dom_alias_add.o dom_alias_rm.o dom_ip_add.o \
	dom_ip_ls.o dom_ip_rm.o dom_rm.o qt_def_get.o qt_def_set.o \
	qt_get.o qt_set.o udot_add.o udot_ls.o udot_rm.o user_add.o \
	user_auth.o user_pass.o user_rm.o autoresp.o error2str.o cdom_name2id.o \
	user_pass.o user_conf_add.o user_conf_ls.o user_conf_rm.o dom_id.o \
	dom_ls.o dom_alias_ls.o user_ls.o user_rep.o
INCS=-I$(BASE)/base -I$(BASE) -I$(BASE)/libsys -I$(BASE)/libtext \
	-I$(BASE)/base/libvqmain -I$(BASE)/libconf \
	-I$(BASE)/corbautil/cxx/import_export -I$(BASE)/corbautil/cxx/portability \
	`cat $(BASE)/corba.inc` `cat $(BASE)/boost.inc`
LIBS=-L$(BASE)/libmain -lmain -L$(BASE)/base/libvqmain -lvqmain -L. -lcluemain \
	-L$(BASE)/libconf -lconf -L$(BASE)/base -lvq -L../../corbautil -lcorbautil \
	`cat $(BASE)/corba.lib`

.PHONY: all clean install

all: .depend libcluemain.a dom_add dom_rm user_add user_rm user_pass \
user_conf_add user_conf_ls \
user_conf_rm user_auth dom_id dom_alias_add dom_alias_rm dom_ls dom_alias_ls \
dom_ip_ls dom_ip_rm dom_ip_add user_ls user_rep

include $(BASE)/Makefile.common

##################

libcluemain.a: $(BASE)/make-lib cluemain.o 
	$(BASE)/make-lib libcluemain.a cluemain.o

dom_add: dom_add.o
dom_rm: dom_rm.o
dom_ls: dom_ls.o

dom_add dom_rm dom_ls: \
$(BASE)/load $(BASE)/corba.lib error2str.o libcluemain.a
	$(BASE)/load $@ error2str.o $(LIBS)

user_ls: user_ls.o
user_add: user_add.o
user_rep: user_rep.o
user_rm: user_rm.o
user_pass: user_pass.o
user_conf_add: user_conf_add.o
user_conf_ls: user_conf_ls.o
user_conf_rm: user_conf_rm.o
dom_id: dom_id.o
dom_alias_add: dom_alias_add.o
dom_alias_rm: dom_alias_rm.o
dom_alias_ls: dom_alias_ls.o
dom_ip_add: dom_ip_add.o
dom_ip_rm: dom_ip_rm.o
dom_ip_ls: dom_ip_ls.o

user_add user_rm user_pass user_conf_add user_conf_ls user_conf_rm dom_id \
dom_alias_add dom_alias_rm dom_alias_ls \
dom_ip_add dom_ip_rm dom_ip_ls user_ls user_rep: \
$(BASE)/load $(BASE)/corba.lib error2str.o cdom_name2id.o libcluemain.a
	$(BASE)/load $@ error2str.o cdom_name2id.o $(LIBS) -L$(BASE)/libtext -ltext

user_auth: user_auth.o

user_auth: $(BASE)/load $(BASE)/corba.lib error2str.o cdom_name2id.o
	$(BASE)/load $@ error2str.o cdom_name2id.o -L$(BASE)/libmain -lmain \
	-L$(BASE)/base/libvqmain -lvqmain \
	-L$(BASE)/libconf -lconf -L$(BASE)/base -lvq \
	-L$(BASE)/libtext -ltext -L$(BASE)/libsys -lsys \
	-L$(BASE)/corbautil -lcorbautil `cat $(BASE)/corba.lib`

$(OBJS): $(BASE)/compile $(BASE)/corba.inc $(BASE)/boost.inc

.cc.o:
	$(BASE)/compile $(INCS) $<

#################
clean:
	rm -rf `cat TARGETS`

#################
install: all
	$(INSTALL) -d -m 0755 $(PREFIX)/bin
	for p in dom_add dom_rm user_add user_rm user_pass \
			user_conf_add user_conf_ls \
			user_conf_rm user_auth dom_id dom_alias_add dom_alias_rm dom_ls dom_alias_ls \
			dom_ip_ls dom_ip_rm dom_ip_add user_ls user_rep ; do \
		$(INSTALL) -m 0755 $$p $(PREFIX)/bin/$$p ; \
	done
	$(INSTALL) -d -m 0755 $(PREFIX)/include/vq/interfaces/clue
	$(INSTALL) -m 0644 cluemain.hpp $(PREFIX)/include/vq/interfaces/clue/cluemain.hpp
	$(INSTALL) -d -m 0755 $(PREFIX)/lib/vq
	$(INSTALL) -m 0755 libcluemain.a $(PREFIX)/lib/vq/libcluemain.a

