all: auto dom_add user_add dom_alias_add user_pass user_rm dom_rm dom_alias_rm \
	user_auth dom_ip_add dom_ip_rm dom_ip_ls pgsqlauthd install qt_set \
	qt_get qt_cur_set qt_over qt_def_set qt_def_get pgsqllogd deliver \
	autoresp udot_add udot_rm udot_ls
#	achk4qmail \

#######################

ac_int: load ac_int.o conf_vq.o
	./load ac_int conf_vq.o

ac_str: load ac_str.o conf_vq.o
	./load ac_str conf_vq.o

achk4qmail: load achk4qmail.o vq_conf.a \
libsys.so vq_init.o cfsvq.o \
cdaemonauth.o libfd.so libtext.so libfs.so
	./load achk4qmail vq_conf.a `cat pgsql.lib` -lfd -ltext -lfs -lsys

auto:
	mkdir auto

autoresp: load autoresp.o hex.o lower.o cqautoresp.o vq_conf.a  \
libsys.so libfd.so
	./load autoresp hex.o lower.o cqautoresp.o vq_conf.a -lsys -lfd

str2cc: str2cc.o load
	./load str2cc

int2cc: int2cc.o load
	./load int2cc

choose: choose.o load
	./load choose

daemon.o: compile daemon.cc daemon.h
	./compile daemon.cc

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

install: load install.o hier.o conf_vq.o vq_conf.a
	./load install conf_vq.o hier.o vq_conf.a

pgsqlauthd: load pgsql.lib pgsqlauthd.o conf_vq.o \
	pgsql.lib pgsqlcommon.o libfd.so libsys.so libfs.so \
	daemon.o vq_conf.a
	./load pgsqlauthd conf_vq.o daemon.o \
	pgsqlcommon.o vq_conf.a `cat pgsql.lib` -lfd -lsys -lfs

pgsqllogd: load pgsql.lib pgsqllogd.o conf_vq.o sig.o \
	pgsql.lib pgsqlcommon.o libfd.so libsys.so libfs.so daemon.o \
	vq_conf.a
	./load pgsqllogd conf_vq.o daemon.o \
	pgsqlcommon.o vq_conf.a `cat pgsql.lib` -lfd -lsys -lfs

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
	libsplit.so hmac_md5.o auto/lmd5.o ccurlog.o ccurlog.lib libtext.so
	./load user_auth vq_conf.a -lvq -lsys -lfd -ltext -lfs -lsplit \
	hmac_md5.o auto/lmd5.o ccurlog.o -ltext `cat ccurlog.lib` 

user_pass: load user_pass.o libvq.so libsplit.so
	./load user_pass -lvq -lsys -lfd -ltext -lfs -lsplit

user_rm: load user_rm.o libvq.so libsplit.so
	./load user_rm -lvq -lsys -lfd -ltext -lfs -lsplit

######################

ac_int.o: compile ac_int.cc conf_vq.h
	./compile ac_int.cc

ac_str.o: compile ac_str.cc conf_vq.h
	./compile ac_str.cc

achk4qmail.o: compile vq_conf.h cconf.h \
sig.h fd.h fdstr.h util.h lock.h achk4qmail.cc \
vq_init.h cvq.h cauth.h sys.h
	./compile achk4qmail.cc `cat pgsql.inc`

autoresp.o: compile autoresp.cc sys.h lower.h cautoresp.h
	./compile autoresp.cc

cconf.o: comp-so cconf.cc cconf.h
	./comp-so cconf.cc

ccurlog.cc: choose conf/log
	ln -sf `./choose \`head -n 1 conf/log\` 1 csyslog.cc 2 cdaemonlog.cc` ccurlog.cc

ccurlog.h: choose conf/log
	ln -sf `./choose \`head -n 1 conf/log\` 1 csyslog.h 2 cdaemonlog.h` ccurlog.h

ccurlog.inc: choose conf/log
	ln -sf `./choose \`head -n 1 conf/log\` 1 csyslog.inc 2 cdaemonlog.inc` ccurlog.inc

ccurlog.lib: choose conf/log
	ln -sf `./choose \`head -n 1 conf/log\` 1 csyslog.lib 2 cdaemonlog.lib` ccurlog.lib

ccurlog.o: compile ccurlog.cc ccurlog.inc ccurlog.h clog.h
	./compile ccurlog.cc `cat ccurlog.inc`
	
cdaemonauth.o: comp-so cdaemonauth.h cdaemonauth.cc cauth.h conf_vq.h \
	fdstr.h fd.h cvq.h lower.h
	./comp-so cdaemonauth.cc

cdaemonlog.o: compile cdaemonlog.h cdaemonlog.cc cauth.h conf_vq.h \
	fdstr.h fd.h cvq.h lower.h
	./compile cdaemonlog.cc

cfsvq.o: comp-so cfsvq.cc cfsvq.h cvq.h lower.h lock.h \
	dirs.h conf_vq.h split.h cauth.h vq_conf.h cconf.h \
	skillall.h auto/d_namlen.h sys.h pfstream.h
	./comp-so cfsvq.cc

choose.o: compile choose.cc
	./compile choose.cc

auto/atchars.cc: ac_str conf/atchars
	head -n 1 conf/atchars | ./ac_str atchars a
	
auto/atchars.o: comp-so auto/atchars.cc cconf.h
	cd auto && ../comp-so atchars.cc

auto/autoresp_ttl.cc: ac_int conf/autoresp_ttl
	head -n 1 conf/autoresp_ttl | ./ac_int autoresp_ttl

auto/autoresp_ttl.o: comp-so auto/autoresp_ttl.cc cconf.h
	cd auto && ../comp-so autoresp_ttl.cc

auto/qmail.cc: ac_str conf/qmail
	head -n 1 conf/qmail | ./ac_str qmail a

auto/qmail.o: comp-so auto/qmail.cc cconf.h
	cd auto && ../comp-so qmail.cc

auto/qmail_hyp.cc: ac_str conf/qmail_hyp
	head -n 1 conf/qmail_hyp | ./ac_str qmail_hyp

auto/qmail_hyp.o: comp-so auto/qmail_hyp.cc cconf.h
	cd auto && ../comp-so qmail_hyp.cc

auto/qmail_mode.cc: ac_int conf/qmail_mode
	head -n 1 conf/qmail_mode | ./ac_int qmail_mode 

auto/qmail_mode.o: comp-so auto/qmail_mode.cc cconf.h
	cd auto && ../comp-so qmail_mode.cc

auto/pgsql.cc: ac_str conf/pgsql
	head -n 1 conf/pgsql | ./ac_str pgsql 

auto/pgsql.o: comp-so auto/pgsql.cc cconf.h
	cd auto && ../comp-so pgsql.cc

auto/ps.cc: ac_str conf/ps
	head -n 1 conf/ps | ./ac_str ps 

auto/ps.o: comp-so auto/ps.cc
	cd auto && ../comp-so ps.cc
	
auto/split_dom.cc: ac_int conf/split_dom
	head -n 1 conf/split_dom | ./ac_int split_dom 

auto/split_dom.o: comp-so auto/split_dom.cc cconf.h
	cd auto && ../comp-so split_dom.cc

auto/split_user.cc: ac_int conf/split_user
	head -n 1 conf/split_user | ./ac_int split_user 

auto/split_user.o: comp-so auto/split_user.cc cconf.h
	cd auto && ../comp-so split_user.cc

conf_vq.cc: str2cc conf/vq
	./str2cc "`head -n 1 conf/vq`" vq a > conf_vq.cc

conf_vq.o: comp-so conf_vq.cc conf_vq.h
	./comp-so conf_vq.cc

auto/vq_fmode.cc: ac_int conf/vq_fmode
	head -n 1 conf/vq_fmode | ./ac_int vq_fmode 
	
auto/vq_fmode.o: comp-so auto/vq_fmode.cc cconf.h
	cd auto && ../comp-so vq_fmode.cc

auto/vq_gid.cc: getgid ac_int conf/vq_user
	./getgid "`head -n 1 conf/vq_user`" | ./ac_int vq_gid 1 cgidconf
	
auto/vq_gid.o: comp-so auto/vq_gid.cc cconf.h
	cd auto && ../comp-so vq_gid.cc

auto/vq_mode.cc: ac_int conf/vq_mode
	head -n 1 conf/vq_mode | ./ac_int vq_mode 
	
auto/vq_mode.o: comp-so auto/vq_mode.cc cconf.h
	cd auto && ../comp-so vq_mode.cc

auto/vq_uid.cc: getuid ac_int conf/vq_user
	./getuid "`head -n 1 conf/vq_user`" | ./ac_int vq_uid 1 cuidconf

auto/vq_uid.o: comp-so auto/vq_uid.cc cconf.h
	cd auto && ../comp-so vq_uid.cc

auto/vq_user.cc: ac_str conf/vq_user
	head -n 1 conf/vq_user | ./ac_str vq_user a

auto/vq_user.o: comp-so auto/vq_user.cc cconf.h
	cd auto && ../comp-so vq_user.cc

cpoll.o: comp-so sys.h cpoll.cc
	./comp-so cpoll.cc

cqautoresp.o: comp-so cqautoresp.h cautoresp.h cqautoresp.cc \
vq_conf.h cconf.h lower.h pfstream.h
	./compile cqautoresp.cc

csyslog.o: compile csyslog.cc csyslog.h clog.h 
	./compile csyslog.cc

cvq.o: comp-so cvq.h cvq.cc
	./comp-so cvq.cc

deliver.o: compile deliver.cc vq_conf.h split.h vq_init.h cvq.h cauth.h
	./compile deliver.cc

dirs.o: comp-so dirs.cc dirs.h
	./comp-so dirs.cc

auto/d_namlen.h: compile try/d_namlen.cc d_namlen.h1 d_namlen.h2
	( ( cd try && ../compile d_namlen.cc ) >/dev/null 2>&1 \
	&& cat d_namlen.h1 || cat d_namlen.h2 ) > auto/d_namlen.h
	rm -f try/d_namlen.o 

dom_add.o: compile dom_add.cc cvq.h cfsvq.h cdaemonauth.h vq_init.h
	./compile dom_add.cc

dom_alias_add.o: compile dom_alias_add.cc cvq.h cfsvq.h cdaemonauth.h vq_init.h  
	./compile dom_alias_add.cc

dom_alias_rm.o: compile dom_alias_rm.cc cvq.h cfsvq.h cdaemonauth.h vq_init.h
	./compile dom_alias_rm.cc

dom_ip_add.o: compile dom_ip_add.cc cvq.h cfsvq.h cdaemonauth.h vq_init.h
	./compile dom_ip_add.cc

dom_ip_ls.o: compile dom_ip_ls.cc cvq.h cfsvq.h cdaemonauth.h vq_init.h
	./compile dom_ip_ls.cc

dom_ip_rm.o: compile dom_ip_rm.cc cvq.h cfsvq.h cdaemonauth.h vq_init.h
	./compile dom_ip_rm.cc

dom_rm.o: compile dom_rm.cc cvq.h cfsvq.h cdaemonauth.h vq_init.h
	./compile dom_rm.cc

fd.o: comp-so fd.h fd.cc
	./comp-so fd.cc

fdstr.o: comp-so fdstr.h fd.h fdstr.cc
	./comp-so fdstr.cc

getgid.o: compile getgid.cc
	./compile getgid.cc

getline.o: comp-so getline.h getline.cc
	./comp-so getline.cc

getuid.o: compile getuid.cc
	./compile getuid.cc

auto/hasflock.h: try/flock.cc compile load
	( ( cd try && ../compile flock.cc && ../load flock ) >/dev/null 2>&1 \
    && echo \#define HASFLOCK 1 || exit 0 ) > auto/hasflock.h
	rm -f try/flock.o try/flock

hex.o: comp-so hex.cc sys.h
	./comp-so hex.cc

hier.o: compile hier.cc install.h conf_vq.h vq_conf.h cconf.h
	./compile hier.cc
	
hmac_md5.o: compile hmac_md5.h auto/lmd5.h
	./compile hmac_md5.c

install.o: compile install.cc install.h
	./compile install.cc
	
int2cc.o: compile int2cc.cc
	./compile int2cc.cc

libfd.so: libfd.so.1
	ln -sf libfd.so.1 libfd.so

libfd.so.1: make-so fd.o fdstr.o getline.o
	./make-so libfd.so.1 fd.o fdstr.o getline.o

libvq.so: libvq.so.1
	ln -sf libvq.so.1 libvq.so

libvq.so.1: make-so cfsvq.o cvq.o conf_vq.o vq_conf.a \
cdaemonauth.o libfd.so libtext.so libsys.so \
libfs.so vq_init.o
	./make-so libvq.so.1 cfsvq.o cvq.o conf_vq.o vq_conf.a \
	cdaemonauth.o vq_init.o -lfd -ltext \
	-lsys -lfs

libsplit.so: libsplit.so.1
	ln -sf libsplit.so.1 libsplit.so

libsplit.so.1: make-so split.o vq_conf.a
	./make-so libsplit.so.1 split.o vq_conf.a

libsys.so: libsys.so.1
	ln -sf libsys.so.1 libsys.so

libsys.so.1: make-so skillall.o sig.o cpoll.o hex.o vq_conf.a 
	./make-so libsys.so.1 skillall.o sig.o cpoll.o hex.o vq_conf.a

libfs.so: libfs.so.1
	ln -sf libfs.so.1 libfs.so

libfs.so.1: make-so util.o lock.o dirs.o
	./make-so libfs.so.1 util.o dirs.o lock.o

libtext.so: libtext.so.1
	ln -sf libtext.so.1 libtext.so

libtext.so.1: make-so lower.o
	./make-so libtext.so.1 lower.o

lock.o: comp-so lock.cc lock.h auto/hasflock.h
	./comp-so lock.cc

lower.o: comp-so lower.cc lower.h
	./comp-so lower.cc

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

pgsqlauthd.o: compile cdaemonauth.h pgsql.inc conf_vq.h \
sig.h fd.h fdstr.h pgsql.inc util.h lock.h pgsqlauthd.cc pgsqlcommon.h \
vq_conf.h cconf.h sys.h daemon.h
	./compile pgsqlauthd.cc `cat pgsql.inc`

pgsqlcommon.o: compile pgsqlcommon.h pgsqlcommon.cc pgsql.inc util.h
	./compile pgsqlcommon.cc `cat pgsql.inc`

pgsqllogd.o: compile cdaemonlog.h pgsql.inc conf_vq.h \
sig.h fd.h fdstr.h pgsql.inc util.h lock.h pgsqllogd.cc pgsqlcommon.h \
sys.h vq_conf.h cconf.h daemon.h
	./compile pgsqllogd.cc `cat pgsql.inc`

qt_cur_set.o: compile qt_cur_set.cc cfsvq.h cdaemonauth.h vq_init.h
	./compile qt_cur_set.cc

qt_def_get.o: compile qt_def_get.cc cfsvq.h cdaemonauth.h vq_init.h
	./compile qt_def_get.cc

qt_def_set.o: compile qt_def_set.cc cfsvq.h cdaemonauth.h  vq_init.h
	./compile qt_def_set.cc

qt_over.o: compile qt_over.cc cfsvq.h cdaemonauth.h  vq_init.h
	./compile qt_over.cc

qt_get.o: compile qt_get.cc cfsvq.h cdaemonauth.h  vq_init.h
	./compile qt_get.cc

qt_set.o: compile qt_set.cc cfsvq.h cdaemonauth.h  vq_init.h
	./compile qt_set.cc

sig.o: comp-so sig.h
	./comp-so sig.cc

skillall.o: comp-so vq_conf.h cconf.h skillall.cc getline.h
	./comp-so skillall.cc

split.o: comp-so split.h vq_conf.h split.cc
	./comp-so split.cc

str2cc.o: compile str2cc.cc
	./compile str2cc.cc

udot_add.o: compile udot_add.cc cfsvq.h cdaemonauth.h vq_init.h
	./compile udot_add.cc

udot_ls.o: compile udot_ls.cc cfsvq.h cdaemonauth.h vq_init.h
	./compile udot_ls.cc

udot_rm.o: compile udot_rm.cc cfsvq.h cdaemonauth.h vq_init.h
	./compile udot_rm.cc

user_add.o: compile user_add.cc cvq.h cfsvq.h cdaemonauth.h vq_init.h
	./compile user_add.cc

user_auth.o: compile user_auth.cc cvq.h cauth.h vq_conf.h cconf.h \
	auto/lmd5.h ccurlog.h ccurlog.inc vq_init.h sig.h sys.h
	./compile user_auth.cc `cat ccurlog.inc`

user_pass.o: compile user_pass.cc cvq.h cfsvq.h cdaemonauth.h vq_init.h
	./compile user_pass.cc

user_rm.o: compile user_add.cc cvq.h cfsvq.h cdaemonauth.h vq_init.h
	./compile user_rm.cc

util.o: comp-so util.cc util.h
	./comp-so util.cc

vq_conf.a: make-lib auto/autoresp_ttl.o auto/atchars.o auto/vq_uid.o \
auto/vq_gid.o auto/qmail.o auto/qmail_mode.o auto/vq_user.o \
auto/vq_mode.o auto/vq_fmode.o auto/qmail_hyp.o auto/pgsql.o \
auto/ps.o auto/split_dom.o auto/split_user.o cconf.o
	./make-lib vq_conf.a auto/autoresp_ttl.o auto/atchars.o auto/vq_uid.o \
	auto/vq_gid.o auto/qmail.o auto/qmail_mode.o auto/vq_user.o \
	auto/vq_mode.o auto/vq_fmode.o auto/qmail_hyp.o auto/pgsql.o \
	auto/ps.o auto/split_dom.o auto/split_user.o cconf.o

vq_init.o: comp-so vq_init.h vq_init.cc
	./comp-so vq_init.cc

#####################
clean:
		rm -rf `cat TARGETS`
