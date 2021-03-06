.PHONY: auto-mkdir

all: auto-mkdir dom_add user_add dom_alias_add user_pass user_rm dom_rm dom_alias_rm \
	user_auth dom_ip_add dom_ip_rm dom_ip_ls pgsqlauthd install qt_set \
	qt_get qt_cur_set qt_over qt_def_set qt_def_get pgsqllogd deliver \
	autoresp udot_add udot_rm udot_ls qmail_assign_add qmail_assign_ex \
	qmail_assign_rm qmail_file_add qmail_file_rm qmail_mrh_add \
	qmail_mrh_rm qmail_rcpthosts_add qmail_run qmail_virtualdomains_add \
	qmail_virtualdomains_rm

#######################

ac_int: load ac_int.o conf_home.o
	./load ac_int conf_home.o

ac_str: load ac_str.o conf_home.o
	./load ac_str conf_home.o

auto-mkdir:
	mkdir -p auto

autoresp: load autoresp.o hex.o lower.o cqautoresp.o vq_conf.a  \
libsys.so libfd.so
	./load autoresp hex.o lower.o cqautoresp.o vq_conf.a -lsys -lfd

str2cc: str2cc.o load
	./load str2cc

choose: choose.o load
	./load choose

deliver: load deliver.o vq_conf.a libsplit.so 
	./load deliver vq_conf.a -lsplit

dom_add: load dom_add.o libvq.so libsplit.so 
	./load dom_add -lvq -lsys -lfd -ltext -lfs -lsplit

dom_alias_add: load dom_alias_add.o libvq.so libsplit.so
	./load dom_alias_add -lvq -lsys -lfd -ltext -lfs -lsplit

dom_alias_rm: load dom_alias_rm.o libvq.so libsplit.so 
	./load dom_alias_rm -lvq -lsys -lfd -ltext -lfs -lsplit

dom_ip_add: load dom_ip_add.o libvq.so libsplit.so
	./load dom_ip_add -lvq -lsys -lfd -ltext -lfs -lsplit

dom_ip_ls: load dom_ip_ls.o libvq.so libsplit.so
	./load dom_ip_ls -lvq -lsys -lfd -ltext -lfs -lsplit

dom_ip_rm: load dom_ip_rm.o libvq.so libsplit.so
	./load dom_ip_rm -lvq -lsys -lfd -ltext -lfs -lsplit

dom_rm: load dom_rm.o libvq.so libsplit.so
	./load dom_rm -lvq -lsys -lfd -ltext -lfs -lsplit

getgid: load getgid.o
	./load getgid

getuid: load getuid.o
	./load getuid

install: load install.o hier.o conf_home.o vq_conf.a
	./load install hier.o vq_conf.a conf_home.o

pgsqlauthd: load pgsql.lib pgsqlauthd.o conf_home.o \
	pgsql.lib pgsqlcommon.o libfd.so libsys.so libfs.so \
	daemon.o vq_conf.a
	./load pgsqlauthd pgsqlcommon.o daemon.o vq_conf.a conf_home.o \
	`cat pgsql.lib` -lfd -lsys -lfs

pgsqllogd: load pgsqllogd.o conf_home.o \
	pgsql.lib pgsqlcommon.o libfd.so libsys.so libfs.so daemon.o \
	vq_conf.a
	./load pgsqllogd pgsqlcommon.o daemon.o vq_conf.a conf_home.o \
	`cat pgsql.lib` -lfd -lsys -lfs

qt_cur_set: load libvq.so libsplit.so qt_cur_set.o
	./load qt_cur_set -lvq -lsys -lfd -ltext -lfs -lsplit

qt_def_get: load libvq.so libsplit.so qt_def_get.o
	./load qt_def_get -lvq -lsys -lfd -ltext -lfs -lsplit

qt_def_set: load libvq.so libsplit.so qt_def_set.o
	./load qt_def_set -lvq -lsys -lfd -ltext -lfs -lsplit

qt_over: load libvq.so libsplit.so qt_over.o
	./load qt_over -lvq -lsys -lfd -ltext -lfs -lsplit

qt_get: load libvq.so libsplit.so qt_get.o
	./load qt_get -lvq -lsys -lfd -ltext -lfs -lsplit

qt_set: load libvq.so libsplit.so qt_set.o
	./load qt_set -lvq -lsys -lfd -ltext -lfs -lsplit

udot_add: load libvq.so libsplit.so udot_add.o
	./load udot_add -lvq -lsys -lfd -ltext -lfs -lsplit

udot_ls: load libvq.so libsplit.so udot_ls.o
	./load udot_ls -lvq -lsys -lfd -ltext -lfs -lsplit

udot_rm: load libvq.so libsplit.so udot_rm.o
	./load udot_rm -lvq -lsys -lfd -ltext -lfs -lsplit

user_add: load user_add.o libvq.so libsplit.so
	./load user_add -lvq -lsys -lfd -ltext -lfs -lsplit

user_auth: load user_auth.o vq_conf.a libvq.so libfd.so libsys.so \
	libsplit.so hmac_md5.o auto/lmd5.o libtext.so liblog.so
	./load user_auth vq_conf.a hmac_md5.o auto/lmd5.o \
	-llog -lvq -lsys -lfd -ltext -lfs -lsplit 

user_pass: load user_pass.o libvq.so libsplit.so
	./load user_pass -lvq -lsys -lfd -ltext -lfs -lsplit

user_rm: load user_rm.o libvq.so libsplit.so
	./load user_rm -lvq -lsys -lfd -ltext -lfs -lsplit

######################

auto/atchars.cc: ac_str conf/atchars
	head -n 1 conf/atchars | ./ac_str vq atchars a
	
auto/atchars.o: comp-so auto/atchars.cc cconf.h
	cd auto && ../comp-so atchars.cc

auto/autoresp_ttl.cc: ac_int conf/autoresp_ttl
	head -n 1 conf/autoresp_ttl | ./ac_int vq autoresp_ttl

auto/autoresp_ttl.o: comp-so auto/autoresp_ttl.cc cconf.h
	cd auto && ../comp-so autoresp_ttl.cc

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

auto/qmail.o: comp-so auto/qmail.cc cconf.h
	cd auto && ../comp-so qmail.cc

auto/qmail_hyp.cc: ac_str conf/qmail_hyp
	head -n 1 conf/qmail_hyp | ./ac_str vq qmail_hyp

auto/qmail_hyp.o: comp-so auto/qmail_hyp.cc cconf.h
	cd auto && ../comp-so qmail_hyp.cc

auto/qmail_mode.cc: ac_int conf/qmail_mode
	head -n 1 conf/qmail_mode | ./ac_int vq qmail_mode 

auto/qmail_mode.o: comp-so auto/qmail_mode.cc cconf.h
	cd auto && ../comp-so qmail_mode.cc

auto/pgsql.cc: ac_str conf/pgsql
	head -n 1 conf/pgsql | ./ac_str vq pgsql 

auto/pgsql.o: comp-so auto/pgsql.cc cconf.h
	cd auto && ../comp-so pgsql.cc

auto/split_dom.cc: ac_int conf/split_dom
	head -n 1 conf/split_dom | ./ac_int vq split_dom 

auto/split_dom.o: comp-so auto/split_dom.cc cconf.h
	cd auto && ../comp-so split_dom.cc

auto/split_user.cc: ac_int conf/split_user
	head -n 1 conf/split_user | ./ac_int vq split_user 

auto/split_user.o: comp-so auto/split_user.cc cconf.h
	cd auto && ../comp-so split_user.cc

auto/vq_fmode.cc: ac_int conf/vq_fmode
	head -n 1 conf/vq_fmode | ./ac_int vq vq_fmode 
	
auto/vq_fmode.o: comp-so auto/vq_fmode.cc cconf.h
	cd auto && ../comp-so vq_fmode.cc

auto/vq_gid.cc: getgid ac_int conf/vq_user
	./getgid "`head -n 1 conf/vq_user`" | ./ac_int vq vq_gid 1 cgidconf
	
auto/vq_gid.o: comp-so auto/vq_gid.cc cconf.h
	cd auto && ../comp-so vq_gid.cc

auto/vq_mode.cc: ac_int conf/vq_mode
	head -n 1 conf/vq_mode | ./ac_int vq vq_mode 
	
auto/vq_mode.o: comp-so auto/vq_mode.cc cconf.h
	cd auto && ../comp-so vq_mode.cc

auto/vq_uid.cc: getuid ac_int conf/vq_user
	./getuid "`head -n 1 conf/vq_user`" | ./ac_int vq vq_uid 1 cuidconf

auto/vq_uid.o: comp-so auto/vq_uid.cc cconf.h
	cd auto && ../comp-so vq_uid.cc

auto/vq_user.cc: ac_str conf/vq_user
	head -n 1 conf/vq_user | ./ac_str vq vq_user a

auto/vq_user.o: comp-so auto/vq_user.cc cconf.h
	cd auto && ../comp-so vq_user.cc

liblog.so: liblog.so.4.0
	ln -sf liblog.so.4.0 liblog.so

liblog.so.4.0: make-so clogger.o cdaemonlog.o
	./make-so liblog.so.4.0 cdaemonlog.o clogger.o

libfd.so: libfd.so.4.0
	ln -sf libfd.so.4.0 libfd.so

libfd.so.4.0: make-so fd.o fdstr.o getline.o uniq.o
	./make-so libfd.so.4.0 fd.o fdstr.o getline.o uniq.o

libvq.so: libvq.so.4.0
	ln -sf libvq.so.4.0 libvq.so

libvq.so.4.0: make-so cvq.o cfsvq.o cfsvq_user.o cfsvq_dom.o conf_home.o vq_conf.a \
cdaemonauth.o cauth.o libfd.so libtext.so libsys.so \
libfs.so
	./make-so libvq.so.4.0 cfsvq.o cfsvq_user.o cfsvq_dom.o cvq.o conf_home.o \
	vq_conf.a cdaemonauth.o cauth.o -lfd -ltext -lsys -lfs

libsplit.so: libsplit.so.4.0
	ln -sf libsplit.so.4.0 libsplit.so

libsplit.so.4.0: make-so split.o vq_conf.a
	./make-so libsplit.so.4.0 split.o vq_conf.a

libsys.so: libsys.so.4.0
	ln -sf libsys.so.4.0 libsys.so

libsys.so.4.0: make-so sig.o cpoll.o hex.o vq_conf.a 
	./make-so libsys.so.4.0 sig.o cpoll.o hex.o vq_conf.a

libfs.so: libfs.so.4.0
	ln -sf libfs.so.4.0 libfs.so

libfs.so.4.0: make-so util.o lock.o dirs.o
	./make-so libfs.so.4.0 util.o dirs.o lock.o

libtext.so: libtext.so.4.0
	ln -sf libtext.so.4.0 libtext.so

libtext.so.4.0: make-so lower.o
	./make-so libtext.so.4.0 lower.o

qmail_assign_add: load qmail_assign_add.o vq_conf.a lock.o uniq.o
	./load qmail_assign_add vq_conf.a lock.o uniq.o

qmail_assign_ex: load qmail_assign_ex.o vq_conf.a conf_home.o
	./load qmail_assign_ex vq_conf.a

qmail_assign_rm: load qmail_assign_rm.o vq_conf.a lock.o uniq.o qmail_common.o
	./load qmail_assign_rm qmail_common.o vq_conf.a lock.o uniq.o

qmail_file_add: load qmail_file_add.o qmail_files.o vq_conf.a lock.o uniq.o
	./load qmail_file_add qmail_files.o vq_conf.a lock.o uniq.o

qmail_file_rm: load qmail_file_rm.o qmail_files.o vq_conf.a lock.o uniq.o \
qmail_common.o
	./load qmail_file_rm qmail_common.o qmail_files.o vq_conf.a lock.o uniq.o

qmail_mrh_add: load qmail_mrh_add.o vq_conf.a lock.o uniq.o qmail_common.o
	./load qmail_mrh_add qmail_common.o vq_conf.a lock.o uniq.o

qmail_mrh_rm: load qmail_mrh_rm.o vq_conf.a lock.o uniq.o qmail_common.o
	./load qmail_mrh_rm qmail_common.o vq_conf.a lock.o uniq.o

qmail_rcpthosts_add: load qmail_rcpthosts_add.o vq_conf.a lock.o uniq.o
	./load qmail_rcpthosts_add vq_conf.a lock.o uniq.o

qmail_run: load qmail_run.o qmail_progs.o vq_conf.a conf_home.o
	./load qmail_run qmail_progs.o vq_conf.a conf_home.o

qmail_virtualdomains_add: load qmail_virtualdomains_add.o vq_conf.a lock.o uniq.o
	./load qmail_virtualdomains_add vq_conf.a lock.o uniq.o

qmail_virtualdomains_rm: load qmail_virtualdomains_rm.o vq_conf.a lock.o uniq.o
	./load qmail_virtualdomains_rm vq_conf.a lock.o uniq.o

ac_int.o: compile ac_int.cc conf_home.h
	./compile ac_int.cc -DHOME_NS=vq

ac_str.o: compile ac_str.cc conf_home.h
	./compile ac_str.cc -DHOME_NS=vq

autoresp.o: compile autoresp.cc lower.h sys.h sig.h cautoresp.h 
	./compile autoresp.cc

cauth.o: comp-so cauth.cc cauth.h cvq.h
	./comp-so cauth.cc

cconf.o: comp-so cconf.cc cconf.h 
	./comp-so cconf.cc

cdaemonauth.o: comp-so cdaemonauth.cc cdaemonauth.h cauth.h cvq.h conf_home.h \
fd.h fdstr.h lower.h
	./comp-so cdaemonauth.cc

cdaemonlog.o: comp-so cdaemonlog.cc cdaemonlog.h clogger.h conf_home.h fd.h \
fdstr.h lower.h
	./comp-so cdaemonlog.cc

cfsvq.o: comp-so cfsvq.cc pfstream.h conf_home.h vq_conf.h cconf.h cfsvq.h \
cvq.h cauth.h lower.h lock.h dirs.h split.h util.h auto/d_namlen.h \
sys.h sig.h qmail_files.h uniq.h
	./comp-so cfsvq.cc

cfsvq_dom.o: comp-so cfsvq_dom.cc pfstream.h conf_home.h vq_conf.h cconf.h \
cfsvq.h cvq.h cauth.h lower.h lock.h dirs.h split.h util.h \
auto/d_namlen.h sys.h sig.h qmail_progs.h 
	./comp-so cfsvq_dom.cc

cfsvq_user.o: comp-so cfsvq_user.cc pfstream.h conf_home.h vq_conf.h cconf.h \
cfsvq.h cvq.h cauth.h lower.h lock.h dirs.h split.h \
util.h auto/d_namlen.h sys.h sig.h
	./comp-so cfsvq_user.cc

choose.o: compile choose.cc
	./compile choose.cc

clogger.o: comp-so clogger.cc clogger.h lower.h
	./comp-so clogger.cc

conf_home.cc: str2cc conf/home
	./str2cc vq "`head -n 1 conf/home`" home a > conf_home.cc	

conf_home.o: comp-so conf_home.cc
	./comp-so conf_home.cc

cpoll.o: comp-so cpoll.cc sys.h sig.h
	./comp-so cpoll.cc

cqautoresp.o: comp-so cqautoresp.cc sys.h sig.h cqautoresp.h lower.h cautoresp.h \
vq_conf.h cconf.h pfstream.h 
	./comp-so cqautoresp.cc

cvq.o: comp-so cvq.cc cvq.h 
	./comp-so cvq.cc

daemon.o: compile daemon.cc daemon.h sys.h  sig.h
	./compile daemon.cc

deliver.o: compile deliver.cc split.h vq_conf.h cconf.h 
	./compile deliver.cc

dirs.o: comp-so dirs.cc dirs.h 
	./comp-so dirs.cc

dom_add.o: compile dom_add.cc cvq.h 
	./compile dom_add.cc

dom_alias_add.o: compile dom_alias_add.cc cvq.h 
	./compile dom_alias_add.cc

dom_alias_rm.o: compile dom_alias_rm.cc cvq.h 
	./compile dom_alias_rm.cc

dom_ip_add.o: compile dom_ip_add.cc cvq.h 
	./compile dom_ip_add.cc

dom_ip_ls.o: compile dom_ip_ls.cc cvq.h 
	./compile dom_ip_ls.cc

dom_ip_rm.o: compile dom_ip_rm.cc cvq.h 
	./compile dom_ip_rm.cc

dom_rm.o: compile dom_rm.cc cvq.h 
	./compile dom_rm.cc

fd.o: comp-so fd.cc fd.h
	./comp-so fd.cc

fdstr.o: comp-so fdstr.cc fdstr.h fd.h
	./comp-so fdstr.cc

getgid.o: compile getgid.cc 
	./compile getgid.cc

getline.o: comp-so getline.cc getline.h 
	./comp-so getline.cc

getuid.o: compile getuid.cc 
	./compile getuid.cc

hex.o: comp-so hex.cc sys.h sig.h
	./comp-so hex.cc

hier.o: compile hier.cc conf_home.h vq_conf.h cconf.h install.h
	./compile hier.cc

hmac_md5.o: comp-so hmac_md5.c auto/lmd5.h
	./comp-so hmac_md5.c

install.o: compile install.cc install.h
	./compile install.cc

lock.o: comp-so lock.cc lock.h auto/hasflock.h
	./comp-so lock.cc

lower.o: comp-so lower.cc lower.h 
	./comp-so lower.cc

pgsqlauthd.o: compile pgsqlauthd.cc conf_home.h vq_conf.h cconf.h \
sig.h fd.h fdstr.h util.h lock.h pgsqlcommon.h sys.h \
daemon.h cvq.h cdaemonauth.h cauth.h pgsql.inc
	./compile pgsqlauthd.cc `cat pgsql.inc`

pgsqlcommon.o: compile pgsqlcommon.cc pgsqlcommon.h util.h pgsql.inc
	./compile pgsqlcommon.cc `cat pgsql.inc`

pgsqllogd.o: compile pgsqllogd.cc cdaemonlog.h clogger.h conf_home.h vq_conf.h \
cconf.h sig.h fd.h fdstr.h util.h lock.h pgsqlcommon.h sys.h \
daemon.h pgsql.inc
	./compile pgsqllogd.cc `cat pgsql.inc`

qmail_assign_add.o: compile qmail_assign_add.cc pfstream.h vq_conf.h cconf.h \
lock.h uniq.h
	./compile qmail_assign_add.cc

qmail_assign_ex.o: compile qmail_assign_ex.cc pfstream.h vq_conf.h cconf.h 
	./compile qmail_assign_ex.cc

qmail_assign_rm.o: compile qmail_assign_rm.cc pfstream.h vq_conf.h cconf.h \
lock.h uniq.h qmail_common.h
	./compile qmail_assign_rm.cc

qmail_common.o: compile qmail_common.cc pfstream.h vq_conf.h cconf.h \
lock.h uniq.h qmail_files.h qmail_common.h
	./compile qmail_common.cc

qmail_file_add.o: compile qmail_file_add.cc pfstream.h vq_conf.h cconf.h \
lock.h uniq.h qmail_files.h
	./compile qmail_file_add.cc

qmail_file_rm.o: compile qmail_file_rm.cc pfstream.h vq_conf.h cconf.h \
lock.h uniq.h qmail_files.h qmail_common.h
	./compile qmail_file_rm.cc

qmail_files.o: comp-so qmail_files.cc qmail_files.h 
	./comp-so qmail_files.cc

qmail_mrh_add.o: compile qmail_mrh_add.cc pfstream.h vq_conf.h cconf.h lock.h uniq.h
	./compile qmail_mrh_add.cc

qmail_mrh_rm.o: compile qmail_mrh_rm.cc pfstream.h vq_conf.h cconf.h \
lock.h uniq.h qmail_common.h
	./compile qmail_mrh_rm.cc

qmail_progs.o: compile qmail_progs.cc qmail_progs.h conf_home.h vq_conf.h \
cconf.h
	./compile qmail_progs.cc

qmail_rcpthosts_add.o: compile qmail_rcpthosts_add.cc pfstream.h vq_conf.h cconf.h \
lock.h uniq.h
	./compile qmail_rcpthosts_add.cc

qmail_run.o: compile qmail_run.cc qmail_progs.h 
	./compile qmail_run.cc

qmail_virtualdomains_add.o: compile qmail_virtualdomains_add.cc pfstream.h \
vq_conf.h cconf.h lock.h uniq.h
	./compile qmail_virtualdomains_add.cc

qmail_virtualdomains_rm.o: compile qmail_virtualdomains_rm.cc pfstream.h \
vq_conf.h cconf.h lock.h uniq.h
	./compile qmail_virtualdomains_rm.cc

qt_cur_set.o: compile qt_cur_set.cc cvq.h
	./compile qt_cur_set.cc

qt_def_get.o: compile qt_def_get.cc cvq.h
	./compile qt_def_get.cc

qt_def_set.o: compile qt_def_set.cc cvq.h
	./compile qt_def_set.cc

qt_get.o: compile qt_get.cc cvq.h 
	./compile qt_get.cc

qt_over.o: compile qt_over.cc cvq.h 
	./compile qt_over.cc

qt_set.o: compile qt_set.cc cvq.h 
	./compile qt_set.cc

sig.o: comp-so sig.cc sig.h
	./comp-so sig.cc

split.o: comp-so split.cc split.h vq_conf.h cconf.h 
	./comp-so split.cc

str2cc.o: compile str2cc.cc 
	./compile str2cc.cc

udot_add.o: compile udot_add.cc cvq.h 
	./compile udot_add.cc

udot_ls.o: compile udot_ls.cc cvq.h 
	./compile udot_ls.cc

udot_rm.o: compile udot_rm.cc cvq.h 
	./compile udot_rm.cc

uniq.o: comp-so uniq.cc uniq.h
	./comp-so uniq.cc

user_add.o: compile user_add.cc cvq.h 
	./compile user_add.cc

user_auth.o: compile user_auth.cc cvq.h getline.h vq_conf.h cconf.h \
lower.h hmac_md5.h auto/lmd5.h clogger.h sig.h sys.h
	./compile user_auth.cc

user_pass.o: compile user_pass.cc cvq.h 
	./compile user_pass.cc

user_rm.o: compile user_rm.cc cvq.h 
	./compile user_rm.cc

util.o: comp-so util.cc util.h 
	./comp-so util.cc

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
