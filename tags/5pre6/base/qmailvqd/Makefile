BASE=../..
OBJS=cqmailvq.o cqmailvq_dom.o cqmailvq_eb.o cqmailvq_user.o \
	cqmailvq_user_conf.o \
	qmail_assign_add.o qmail_assign_ex.o qmail_assign_rm.o qmail_common.o \
	qmail_file_add.o \
	qmail_file_rm.o qmail_files.o qmail_mrh_add.o qmail_mrh_rm.o \
	qmail_rcpthosts_add.o \
	qmail_virtualdomains_add.o qmail_virtualdomains_rm.o qmailvqd.o \
	cqautoresp.o autoresp.o
INCS=-I$(BASE)/base -I$(BASE) -I$(BASE)/libvqmain -I$(BASE)/libsys -I$(BASE)/libtext \
	-I$(BASE)/libconf `cat $(BASE)/corba.inc`
LIBS=-L$(BASE) -L$(BASE)/libmain -lmain \
	-L$(BASE)/libvqmain -lvqmain -L$(BASE)/base -lvq \
	-L$(BASE)/libconf -lconf \
	-L$(BASE)/libsys -lsys -L$(BASE)/libtext -ltext

all: .depend auto-mkdir auto/d_namlen.h qmailvqd qmail_virtualdomains_add \
qmail_virtualdomains_rm qmail_rcpthosts_add qmail_mrh_add qmail_mrh_rm \
qmail_assign_add qmail_assign_ex qmail_assign_rm qmail_file_add \
qmail_file_rm autoresp

include $(BASE)/Makefile.common

qmailvqd: $(BASE)/load $(BASE)/corba.lib qmail_files.o qmailvqd.o \
cqmailvq_dom.o cqmailvq.o cqmailvq_user.o cqmailvq_eb.o cqmailvq_user_conf.o
	$(BASE)/load qmailvqd qmail_files.o cqmailvq_dom.o cqmailvq.o \
	cqmailvq_user.o cqmailvq_eb.o cqmailvq_user_conf.o \
	$(LIBS) `cat $(BASE)/corba.lib`

qmail_virtualdomains_add: $(BASE)/load qmail_virtualdomains_add.o
	$(BASE)/load qmail_virtualdomains_add $(LIBS)

qmail_virtualdomains_rm: $(BASE)/load qmail_virtualdomains_rm.o
	$(BASE)/load qmail_virtualdomains_rm $(LIBS)

qmail_rcpthosts_add: $(BASE)/load qmail_rcpthosts_add.o
	$(BASE)/load qmail_rcpthosts_add $(LIBS)

qmail_mrh_add: $(BASE)/load qmail_mrh_add.o
	$(BASE)/load qmail_mrh_add $(LIBS)

qmail_mrh_rm: $(BASE)/load qmail_mrh_rm.o qmail_common.o
	$(BASE)/load qmail_mrh_rm qmail_common.o $(LIBS)

qmail_assign_add: $(BASE)/load qmail_assign_add.o
	$(BASE)/load qmail_assign_add $(LIBS)

qmail_assign_rm: $(BASE)/load qmail_assign_rm.o qmail_common.o
	$(BASE)/load qmail_assign_rm qmail_common.o $(LIBS)

qmail_assign_ex: $(BASE)/load qmail_assign_ex.o
	$(BASE)/load qmail_assign_ex $(LIBS)

qmail_file_add: $(BASE)/load qmail_file_add.o qmail_files.o
	$(BASE)/load qmail_file_add qmail_files.o $(LIBS)

qmail_file_rm: $(BASE)/load qmail_file_rm.o qmail_files.o qmail_common.o
	$(BASE)/load qmail_file_rm qmail_files.o qmail_common.o $(LIBS)

autoresp: $(BASE)/load autoresp.o cqautoresp.o ../libautoresp.a
	$(BASE)/load autoresp cqautoresp.o $(LIBS) -L.. -lautoresp 

auto/d_namlen.h: $(BASE)/compile try/d_namlen.cc d_namlen.h1 d_namlen.h2
	( ( cd try && $(BASE)/compile d_namlen.cc ) >/dev/null 2>&1 \
	&& cat d_namlen.h1 || cat d_namlen.h2 ) > auto/d_namlen.h
	rm -f try/d_namlen.o

##################

qmail_files.o qmailvqd.o cqmailvq_dom.o cqmailvq.o cqmailvq_user.o \
cqmailvq_eb.o cqmailvq_user_conf.o: $(BASE)/corba.inc

$(OBJS): $(BASE)/compile

cqmailvq_user.o: auto/d_namlen.h

.cc.o:
	$(BASE)/compile $(INCS) $<

#################
clean:
		rm -rf `cat TARGETS`

