BASE=../..
CORE=..

.PHONY: auto-mkdir

all: auto-mkdir libpgsqlauthd.so libpgsqllogd.so

#######################

ac_int: load main.o ac_int.o conf_home.o
	./load ac_int main.o conf_home.o

ac_str: load ac_str.o main.o conf_home.o
	./load ac_str main.o conf_home.o

auto-mkdir:
	mkdir -p auto

auto/dl.lib: try/dl.c compile load
	( cd try && ../compile dl.c && ../load dl -ldl >/dev/null 2>&1 \
	&& echo -ldl || echo "" ) > auto/dl.lib
	rm -f try/dl.o try/dl

autoresp: load main.o autoresp.o hex.o lower.o cqautoresp.o vq_conf.a  \
libsys.so libfd.so
	./load autoresp main.o hex.o lower.o cqautoresp.o vq_conf.a -lsys -lfd

str2cc: str2cc.o load main.o
	./load str2cc main.o

choose: choose.o load main.o
	./load choose main.o

deliver: load deliver.o vq_conf.a libsplit.so main.o
	./load deliver main.o vq_conf.a -lsplit

dom_add: load main.o dom_add.o libvq.so libsplit.so 
	./load dom_add main.o -lvq -lsys -lfd -ltext -lfs -lsplit

dom_alias_add: load main.o dom_alias_add.o libvq.so libsplit.so
	./load dom_alias_add main.o -lvq -lsys -lfd -ltext -lfs -lsplit

dom_alias_rm: load main.o dom_alias_rm.o libvq.so libsplit.so 
	./load dom_alias_rm main.o -lvq -lsys -lfd -ltext -lfs -lsplit

dom_ip_add: load main.o dom_ip_add.o libvq.so libsplit.so
	./load dom_ip_add main.o -lvq -lsys -lfd -ltext -lfs -lsplit

dom_ip_ls: load main.o dom_ip_ls.o libvq.so libsplit.so
	./load dom_ip_ls main.o -lvq -lsys -lfd -ltext -lfs -lsplit

dom_ip_rm: load main.o dom_ip_rm.o libvq.so libsplit.so
	./load dom_ip_rm main.o -lvq -lsys -lfd -ltext -lfs -lsplit

dom_rm: load main.o dom_rm.o libvq.so libsplit.so
	./load dom_rm main.o -lvq -lsys -lfd -ltext -lfs -lsplit

getgid: load main.o getgid.o
	./load getgid main.o 

getuid: load getuid.o
	./load getuid main.o

install: load main.o install.o hier.o conf_home.o vq_conf.a
	./load install main.o hier.o vq_conf.a conf_home.o

libpgsqlauthd.so: $(BASE)/make-so pgsql.lib pgsqlauthd.o conf_home.o \
	pgsql.lib pgsqlcommon.o libfd.so libsys.so libfs.so \
	vq_conf.a libtext.so
	$(BASE)/make-so libpgsqlauthd.so pgsqlauthd.o pgsqlcommon.o vq_conf.a conf_home.o \
	`cat pgsql.lib` -lfd -lsys -lfs -ltext

libpgsqllogd.so: $(BASE)/make-so pgsqllogd.o conf_home.o \
	pgsql.lib pgsqlcommon.o libfd.so libsys.so libfs.so \
	vq_conf.a libtext.so
	$(BASE)/make-so libpgsqllogd.so pgsqllogd.o pgsqlcommon.o vq_conf.a conf_home.o \
	`cat pgsql.lib` -lfd -lsys -lfs -ltext

qt_cur_set: load main.o libvq.so libsplit.so qt_cur_set.o
	./load qt_cur_set main.o -lvq -lsys -lfd -ltext -lfs -lsplit

qt_def_get: load main.o libvq.so libsplit.so qt_def_get.o
	./load qt_def_get main.o -lvq -lsys -lfd -ltext -lfs -lsplit

qt_def_set: load main.o libvq.so libsplit.so qt_def_set.o
	./load qt_def_set main.o -lvq -lsys -lfd -ltext -lfs -lsplit

qt_over: load main.o libvq.so libsplit.so qt_over.o
	./load qt_over main.o -lvq -lsys -lfd -ltext -lfs -lsplit

qt_get: load libvq.so main.o libsplit.so qt_get.o
	./load qt_get main.o -lvq -lsys -lfd -ltext -lfs -lsplit

qt_set: load libvq.so main.o libsplit.so qt_set.o
	./load qt_set main.o -lvq -lsys -lfd -ltext -lfs -lsplit

udot_add: load main.o libvq.so libsplit.so udot_add.o
	./load udot_add main.o -lvq -lsys -lfd -ltext -lfs -lsplit

udot_ls: load main.o libvq.so libsplit.so udot_ls.o
	./load udot_ls main.o -lvq -lsys -lfd -ltext -lfs -lsplit

udot_rm: load main.o libvq.so libsplit.so udot_rm.o
	./load udot_rm main.o -lvq -lsys -lfd -ltext -lfs -lsplit

user_add: load main.o user_add.o libvq.so libsplit.so
	./load user_add main.o -lvq -lsys -lfd -ltext -lfs -lsplit

user_auth: load main.o user_auth.o vq_conf.a libvq.so libfd.so libsys.so \
	libsplit.so hmac_md5.o auto/lmd5.o libtext.so liblog.so
	./load user_auth main.o vq_conf.a hmac_md5.o auto/lmd5.o \
	-llog -lvq -lsys -lfd -ltext -lfs -lsplit 

user_pass: load main.o user_pass.o libvq.so libsplit.so
	./load user_pass main.o -lvq -lsys -lfd -ltext -lfs -lsplit

user_rm: load main.o user_rm.o libvq.so libsplit.so
	./load user_rm main.o -lvq -lsys -lfd -ltext -lfs -lsplit

######################

auto/atchars.cc: ac_str conf/atchars
	head -n 1 conf/atchars | ./ac_str vq atchars a
	
auto/atchars.o: $(BASE)/comp-so auto/atchars.cc cconf.h
	cd auto && .$(BASE)/comp-so atchars.cc

auto/autoresp_ttl.cc: ac_int conf/autoresp_ttl
	head -n 1 conf/autoresp_ttl | ./ac_int vq autoresp_ttl

auto/autoresp_ttl.o: $(BASE)/comp-so auto/autoresp_ttl.cc cconf.h
	cd auto && .$(BASE)/comp-so autoresp_ttl.cc

auto/d_namlen.h: compile try/d_namlen.cc d_namlen.h1 d_namlen.h2
	( ( cd try && ../compile d_namlen.cc ) >/dev/null 2>&1 \
	&& cat d_namlen.h1 || cat d_namlen.h2 ) > auto/d_namlen.h
	rm -f try/d_namlen.o 

auto/hasflock.h: try/flock.cc compile load
	( ( cd try && ../compile flock.cc && ../load flock ) >/dev/null 2>&1 \
    && echo \#define HASFLOCK 1 || exit 0 ) > auto/hasflock.h
	rm -f try/flock.o try/flock

auto/lmd5.c: compile try/md5.c md5.c1 md5.c2
	( ( cd try && ../compile md5.c ) >/dev/null 2>&1 \
	&& cat md5.c1 || cat md5.c2 ) > auto/lmd5.c
	rm -f try/md5.o 

auto/lmd5.h: compile try/md5.c md5.h1 md5.h2
	( ( cd try && ./compile md5.c ) >/dev/null 2>&1 \
	&& cat md5.h1 || cat md5.h2 ) > auto/lmd5.h
	rm -f try/md5.o

auto/lmd5.o: compile auto/lmd5.c auto/lmd5.h
	(cd auto && ../compile lmd5.c)

auto/qmail.cc: ac_str conf/qmail
	head -n 1 conf/qmail | ./ac_str vq qmail a

auto/qmail.o: $(BASE)/comp-so auto/qmail.cc cconf.h
	cd auto && .$(BASE)/comp-so qmail.cc

auto/qmail_hyp.cc: ac_str conf/qmail_hyp
	head -n 1 conf/qmail_hyp | ./ac_str vq qmail_hyp

auto/qmail_hyp.o: $(BASE)/comp-so auto/qmail_hyp.cc cconf.h
	cd auto && .$(BASE)/comp-so qmail_hyp.cc

auto/qmail_mode.cc: ac_int conf/qmail_mode
	head -n 1 conf/qmail_mode | ./ac_int vq qmail_mode 

auto/qmail_mode.o: $(BASE)/comp-so auto/qmail_mode.cc cconf.h
	cd auto && .$(BASE)/comp-so qmail_mode.cc

auto/pgsql.cc: ac_str conf/pgsql
	head -n 1 conf/pgsql | ./ac_str vq pgsql 

auto/pgsql.o: $(BASE)/comp-so auto/pgsql.cc cconf.h
	cd auto && .$(BASE)/comp-so pgsql.cc

auto/split_dom.cc: ac_int conf/split_dom
	head -n 1 conf/split_dom | ./ac_int vq split_dom 

auto/split_dom.o: $(BASE)/comp-so auto/split_dom.cc cconf.h
	cd auto && .$(BASE)/comp-so split_dom.cc

auto/split_user.cc: ac_int conf/split_user
	head -n 1 conf/split_user | ./ac_int vq split_user 

auto/split_user.o: $(BASE)/comp-so auto/split_user.cc cconf.h
	cd auto && .$(BASE)/comp-so split_user.cc

auto/vq_fmode.cc: ac_int conf/vq_fmode
	head -n 1 conf/vq_fmode | ./ac_int vq vq_fmode 
	
auto/vq_fmode.o: $(BASE)/comp-so auto/vq_fmode.cc cconf.h
	cd auto && .$(BASE)/comp-so vq_fmode.cc

auto/vq_gid.cc: getgid ac_int conf/vq_user
	./getgid "`head -n 1 conf/vq_user`" | ./ac_int vq vq_gid 1 cgidconf
	
auto/vq_gid.o: $(BASE)/comp-so auto/vq_gid.cc cconf.h
	cd auto && .$(BASE)/comp-so vq_gid.cc

auto/vq_mode.cc: ac_int conf/vq_mode
	head -n 1 conf/vq_mode | ./ac_int vq vq_mode 
	
auto/vq_mode.o: $(BASE)/comp-so auto/vq_mode.cc cconf.h
	cd auto && .$(BASE)/comp-so vq_mode.cc

auto/vq_uid.cc: getuid ac_int conf/vq_user
	./getuid "`head -n 1 conf/vq_user`" | ./ac_int vq vq_uid 1 cuidconf

auto/vq_uid.o: $(BASE)/comp-so auto/vq_uid.cc cconf.h
	cd auto && .$(BASE)/comp-so vq_uid.cc

auto/vq_user.cc: ac_str conf/vq_user
	head -n 1 conf/vq_user | ./ac_str vq vq_user a

auto/vq_user.o: $(BASE)/comp-so auto/vq_user.cc cconf.h
	cd auto && .$(BASE)/comp-so vq_user.cc

ben: load ben.o main.o libsys.so libvq.so libsplit.so pthread.lib
	./load ben main.o -lsys -lvq -lsplit `cat pthread.lib`

ben.o: compile ben.cc sys.h sig.h cdaemonauth.h cauth.h cvq.h main.h
	./compile ben.cc

liblog.so: liblog.so.5.0
	ln -sf liblog.so.5.0 liblog.so

liblog.so.5.0: $(BASE)/make-so clogger.o cdaemonlog.o
	$(BASE)/make-so liblog.so.5.0 cdaemonlog.o clogger.o

libfd.so: libfd.so.5.0
	ln -sf libfd.so.5.0 libfd.so

libfd.so.5.0: $(BASE)/make-so fd.o fdstr.o getline.o uniq.o
	$(BASE)/make-so libfd.so.5.0 fd.o fdstr.o getline.o uniq.o

libvq.so: libvq.so.5.0
	ln -sf libvq.so.5.0 libvq.so

libvq.so.5.0: $(BASE)/make-so cvq.o cfsvq.o cfsvq_user.o cfsvq_dom.o conf_home.o vq_conf.a \
cdaemonauth.o cauth.o libfd.so libtext.so libsys.so \
libfs.so
	$(BASE)/make-so libvq.so.5.0 cfsvq.o cfsvq_user.o cfsvq_dom.o cvq.o conf_home.o \
	vq_conf.a cdaemonauth.o cauth.o -lfd -ltext -lsys -lfs

libsplit.so: libsplit.so.5.0
	ln -sf libsplit.so.5.0 libsplit.so

libsplit.so.5.0: $(BASE)/make-so split.o vq_conf.a
	$(BASE)/make-so libsplit.so.5.0 split.o vq_conf.a

libsys.so: libsys.so.5.0
	ln -sf libsys.so.5.0 libsys.so

libsys.so.5.0: $(BASE)/make-so sig.o cpoll.o hex.o vq_conf.a 
	$(BASE)/make-so libsys.so.5.0 sig.o cpoll.o hex.o vq_conf.a

libfs.so: libfs.so.5.0
	ln -sf libfs.so.5.0 libfs.so

libfs.so.5.0: $(BASE)/make-so util.o lock.o dirs.o
	$(BASE)/make-so libfs.so.5.0 util.o dirs.o lock.o

libtext.so: libtext.so.5.0
	ln -sf libtext.so.5.0 libtext.so

libtext.so.5.0: $(BASE)/make-so lower.o
	$(BASE)/make-so libtext.so.5.0 lower.o

qmail_assign_add: load main.o qmail_assign_add.o vq_conf.a lock.o uniq.o
	./load qmail_assign_add main.o vq_conf.a lock.o uniq.o

qmail_assign_ex: load main.o qmail_assign_ex.o vq_conf.a conf_home.o
	./load qmail_assign_ex main.o vq_conf.a

qmail_assign_rm: load main.o qmail_assign_rm.o vq_conf.a lock.o uniq.o qmail_common.o
	./load qmail_assign_rm main.o qmail_common.o vq_conf.a lock.o uniq.o

qmail_file_add: load main.o qmail_file_add.o qmail_files.o vq_conf.a lock.o uniq.o
	./load qmail_file_add main.o qmail_files.o vq_conf.a lock.o uniq.o

qmail_file_rm: load main.o qmail_file_rm.o qmail_files.o vq_conf.a lock.o uniq.o \
qmail_common.o
	./load qmail_file_rm main.o qmail_common.o qmail_files.o vq_conf.a lock.o uniq.o

qmail_mrh_add: load main.o qmail_mrh_add.o vq_conf.a lock.o uniq.o qmail_common.o
	./load qmail_mrh_add main.o qmail_common.o vq_conf.a lock.o uniq.o

qmail_mrh_rm: load main.o qmail_mrh_rm.o vq_conf.a lock.o uniq.o qmail_common.o
	./load qmail_mrh_rm main.o qmail_common.o vq_conf.a lock.o uniq.o

qmail_rcpthosts_add: load main.o qmail_rcpthosts_add.o vq_conf.a lock.o uniq.o
	./load qmail_rcpthosts_add main.o vq_conf.a lock.o uniq.o

qmail_run: load main.o qmail_run.o qmail_progs.o vq_conf.a conf_home.o
	./load qmail_run main.o qmail_progs.o vq_conf.a conf_home.o

qmail_virtualdomains_add: load main.o qmail_virtualdomains_add.o vq_conf.a lock.o uniq.o
	./load qmail_virtualdomains_add main.o vq_conf.a lock.o uniq.o

qmail_virtualdomains_rm: load main.o qmail_virtualdomains_rm.o vq_conf.a lock.o uniq.o
	./load qmail_virtualdomains_rm main.o vq_conf.a lock.o uniq.o

ac_int.o: compile ac_int.cc conf_home.h main.h
	./compile ac_int.cc -DHOME_NS=vq

ac_str.o: compile ac_str.cc conf_home.h main.h
	./compile ac_str.cc -DHOME_NS=vq

autoresp.o: compile autoresp.cc lower.h sys.h sig.h cautoresp.h main.h
	./compile autoresp.cc

cauth.o: $(BASE)/comp-so cauth.cc cauth.h cvq.h
	$(BASE)/comp-so cauth.cc

cconf.o: $(BASE)/comp-so cconf.cc cconf.h 
	$(BASE)/comp-so cconf.cc

cdaemonauth.o: $(BASE)/comp-so cdaemonauth.cc cdaemonauth.h cauth.h cvq.h conf_home.h \
fd.h fdstr.h sys.h sig.h
	$(BASE)/comp-so cdaemonauth.cc

cdaemonlog.o: $(BASE)/comp-so cdaemonlog.cc cdaemonlog.h clogger.h conf_home.h fd.h \
fdstr.h lower.h sys.h sig.h
	$(BASE)/comp-so cdaemonlog.cc

cforkmaster.o: compile sys.h sig.h cforkmaster.cc cforkmaster.h \
cdaemonmaster.h cdaemonmaster.h lock.h cdaemonchild.h
	./compile cforkmaster.cc

cfsvq.o: $(BASE)/comp-so cfsvq.cc pfstream.h conf_home.h vq_conf.h cconf.h cfsvq.h \
cvq.h cauth.h lower.h lock.h dirs.h split.h util.h auto/d_namlen.h \
sys.h sig.h qmail_files.h uniq.h
	$(BASE)/comp-so cfsvq.cc

cfsvq_dom.o: $(BASE)/comp-so cfsvq_dom.cc pfstream.h conf_home.h vq_conf.h cconf.h \
cfsvq.h cvq.h cauth.h lower.h lock.h dirs.h split.h util.h \
auto/d_namlen.h sys.h sig.h qmail_progs.h 
	$(BASE)/comp-so cfsvq_dom.cc

cfsvq_user.o: $(BASE)/comp-so cfsvq_user.cc pfstream.h conf_home.h vq_conf.h cconf.h \
cfsvq.h cvq.h cauth.h lower.h lock.h dirs.h split.h \
util.h auto/d_namlen.h sys.h sig.h
	$(BASE)/comp-so cfsvq_user.cc

choose.o: compile choose.cc main.h
	./compile choose.cc

clogger.o: $(BASE)/comp-so clogger.cc clogger.h lower.h
	$(BASE)/comp-so clogger.cc

conf_home.cc: str2cc conf/home
	./str2cc vq "`head -n 1 conf/home`" home a > conf_home.cc	

conf_home.o: $(BASE)/comp-so conf_home.cc
	$(BASE)/comp-so conf_home.cc

cpoll.o: $(BASE)/comp-so cpoll.cc sys.h sig.h
	$(BASE)/comp-so cpoll.cc

cqautoresp.o: $(BASE)/comp-so cqautoresp.cc sys.h sig.h cqautoresp.h lower.h cautoresp.h \
vq_conf.h cconf.h pfstream.h 
	$(BASE)/comp-so cqautoresp.cc

cvq.o: $(BASE)/comp-so cvq.cc cvq.h 
	$(BASE)/comp-so cvq.cc

daemon: main.o daemon.o cforkmaster.o libsys.so libfs.so auto/dl.lib
	./load daemon main.o cforkmaster.o -lsys -lfs `cat auto/dl.lib`

daemon.o: daemon.cc cdaemonchild.h cdaemonmaster.h main.h
	./compile daemon.cc

deliver.o: compile deliver.cc split.h vq_conf.h cconf.h main.h
	./compile deliver.cc

dirs.o: $(BASE)/comp-so dirs.cc dirs.h main.h
	$(BASE)/comp-so dirs.cc

dom_add.o: compile dom_add.cc cvq.h main.h
	./compile dom_add.cc

dom_alias_add.o: compile dom_alias_add.cc cvq.h main.h
	./compile dom_alias_add.cc

dom_alias_rm.o: compile dom_alias_rm.cc cvq.h main.h
	./compile dom_alias_rm.cc

dom_ip_add.o: compile dom_ip_add.cc cvq.h main.h
	./compile dom_ip_add.cc

dom_ip_ls.o: compile dom_ip_ls.cc cvq.h main.h
	./compile dom_ip_ls.cc

dom_ip_rm.o: compile dom_ip_rm.cc cvq.h main.h
	./compile dom_ip_rm.cc

dom_rm.o: compile dom_rm.cc cvq.h main.h
	./compile dom_rm.cc

fd.o: $(BASE)/comp-so fd.cc fd.h
	$(BASE)/comp-so fd.cc

fdstr.o: $(BASE)/comp-so fdstr.cc fdstr.h fd.h
	$(BASE)/comp-so fdstr.cc

getgid.o: compile getgid.cc main.h
	./compile getgid.cc

getline.o: $(BASE)/comp-so getline.cc getline.h 
	$(BASE)/comp-so getline.cc

getuid.o: compile getuid.cc main.h
	./compile getuid.cc

hex.o: $(BASE)/comp-so hex.cc sys.h sig.h
	$(BASE)/comp-so hex.cc

hier.o: compile hier.cc conf_home.h vq_conf.h cconf.h install.h
	./compile hier.cc

hmac_md5.o: $(BASE)/comp-so hmac_md5.c auto/lmd5.h
	$(BASE)/comp-so hmac_md5.c

install.o: compile install.cc install.h main.h
	./compile install.cc

lock.o: $(BASE)/comp-so lock.cc lock.h auto/hasflock.h
	$(BASE)/comp-so lock.cc

lower.o: $(BASE)/comp-so lower.cc lower.h 
	$(BASE)/comp-so lower.cc

maildir_inject.o: maildir_inject.cc compile
	./compile maildir_inject.cc

main.o: compile main.h main.cc
	./compile main.cc

pgsqlauthd.o: $(BASE)/comp-so pgsqlauthd.cc vq_conf.h cconf.h \
sig.h fd.h fdstr.h util.h lock.h pgsqlcommon.h sys.h \
cvq.h cdaemonauth.h cauth.h pgsql.inc cdaemonchild.h cdaemonmaster.h lower.h
	$(BASE)/comp-so pgsqlauthd.cc `cat pgsql.inc`

pgsqlcommon.o: $(BASE)/comp-so pgsqlcommon.cc pgsqlcommon.h util.h pgsql.inc
	$(BASE)/comp-so pgsqlcommon.cc `cat pgsql.inc`

pgsqllogd.o: compile pgsqllogd.cc cdaemonlog.h clogger.h conf_home.h vq_conf.h \
cconf.h sig.h fd.h fdstr.h util.h lock.h pgsqlcommon.h sys.h \
cdaemonauth.h pgsql.inc cdaemonchild.h cdaemonmaster.h
	./compile pgsqllogd.cc `cat pgsql.inc`

qmail_assign_add.o: compile qmail_assign_add.cc pfstream.h vq_conf.h cconf.h \
lock.h uniq.h main.h
	./compile qmail_assign_add.cc

qmail_assign_ex.o: compile qmail_assign_ex.cc pfstream.h vq_conf.h cconf.h \
main.h
	./compile qmail_assign_ex.cc

qmail_assign_rm.o: compile qmail_assign_rm.cc pfstream.h vq_conf.h cconf.h \
lock.h uniq.h qmail_common.h main.h
	./compile qmail_assign_rm.cc

qmail_common.o: compile qmail_common.cc pfstream.h vq_conf.h cconf.h \
lock.h uniq.h qmail_files.h qmail_common.h
	./compile qmail_common.cc

qmail_file_add.o: compile qmail_file_add.cc pfstream.h vq_conf.h cconf.h \
lock.h uniq.h qmail_files.h main.h
	./compile qmail_file_add.cc

qmail_file_rm.o: compile qmail_file_rm.cc pfstream.h vq_conf.h cconf.h \
lock.h uniq.h qmail_files.h qmail_common.h main.h
	./compile qmail_file_rm.cc

qmail_files.o: $(BASE)/comp-so qmail_files.cc qmail_files.h 
	$(BASE)/comp-so qmail_files.cc

qmail_mrh_add.o: compile qmail_mrh_add.cc pfstream.h vq_conf.h cconf.h lock.h \
uniq.h main.h
	./compile qmail_mrh_add.cc

qmail_mrh_rm.o: compile qmail_mrh_rm.cc pfstream.h vq_conf.h cconf.h \
lock.h uniq.h qmail_common.h main.h
	./compile qmail_mrh_rm.cc

qmail_progs.o: compile qmail_progs.cc qmail_progs.h conf_home.h vq_conf.h \
cconf.h
	./compile qmail_progs.cc

qmail_rcpthosts_add.o: compile qmail_rcpthosts_add.cc pfstream.h vq_conf.h cconf.h \
lock.h uniq.h main.h
	./compile qmail_rcpthosts_add.cc

qmail_run.o: compile qmail_run.cc qmail_progs.h main.h
	./compile qmail_run.cc

qmail_virtualdomains_add.o: compile qmail_virtualdomains_add.cc pfstream.h \
vq_conf.h cconf.h lock.h uniq.h main.h
	./compile qmail_virtualdomains_add.cc

qmail_virtualdomains_rm.o: compile qmail_virtualdomains_rm.cc pfstream.h \
vq_conf.h cconf.h lock.h uniq.h main.h
	./compile qmail_virtualdomains_rm.cc

qt_cur_set.o: compile qt_cur_set.cc cvq.h main.h
	./compile qt_cur_set.cc

qt_def_get.o: compile qt_def_get.cc cvq.h main.h
	./compile qt_def_get.cc

qt_def_set.o: compile qt_def_set.cc cvq.h main.h
	./compile qt_def_set.cc

qt_get.o: compile qt_get.cc cvq.h  main.h
	./compile qt_get.cc

qt_over.o: compile qt_over.cc cvq.h  main.h
	./compile qt_over.cc

qt_set.o: compile qt_set.cc cvq.h  main.h
	./compile qt_set.cc

sig.o: $(BASE)/comp-so sig.cc sig.h
	$(BASE)/comp-so sig.cc

split.o: $(BASE)/comp-so split.cc split.h vq_conf.h cconf.h  main.h
	$(BASE)/comp-so split.cc

str2cc.o: compile str2cc.cc main.h
	./compile str2cc.cc

udot_add.o: compile udot_add.cc cvq.h main.h
	./compile udot_add.cc

udot_ls.o: compile udot_ls.cc cvq.h main.h
	./compile udot_ls.cc

udot_rm.o: compile udot_rm.cc cvq.h main.h
	./compile udot_rm.cc

uniq.o: $(BASE)/comp-so uniq.cc uniq.h
	$(BASE)/comp-so uniq.cc

user_add.o: compile user_add.cc cvq.h main.h
	./compile user_add.cc

user_auth.o: compile user_auth.cc cvq.h getline.h vq_conf.h cconf.h \
lower.h hmac_md5.h auto/lmd5.h clogger.h sig.h sys.h main.h
	./compile user_auth.cc

user_pass.o: compile user_pass.cc cvq.h main.h
	./compile user_pass.cc

user_rm.o: compile user_rm.cc cvq.h main.h
	./compile user_rm.cc

util.o: $(BASE)/comp-so util.cc util.h 
	$(BASE)/comp-so util.cc

vq_conf.a: make-lib auto/autoresp_ttl.o auto/atchars.o auto/vq_uid.o \
auto/vq_gid.o auto/qmail.o auto/qmail_mode.o auto/vq_user.o \
auto/vq_mode.o auto/vq_fmode.o auto/qmail_hyp.o auto/pgsql.o \
auto/split_dom.o auto/split_user.o cconf.o
	./make-lib vq_conf.a auto/autoresp_ttl.o auto/atchars.o auto/vq_uid.o \
	auto/vq_gid.o auto/qmail.o auto/qmail_mode.o auto/vq_user.o \
	auto/vq_mode.o auto/vq_fmode.o auto/qmail_hyp.o auto/pgsql.o \
	auto/split_dom.o auto/split_user.o cconf.o

#####################
clean:
		rm -rf `cat TARGETS`
