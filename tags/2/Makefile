all: dom_add user_add dom_alias_add user_pass user_rm dom_rm dom_alias_rm \
	user_auth dom_ip_add dom_ip_rm dom_ip_ls pgsqlauthd install qt_set \
	qt_get qt_cur_set qt_over qt_def_set qt_def_get pgsqllogd deliver \
#	achk4qmail \
	#udot_add \
	udot_rm udot_ls

#######################

achk4qmail: load achk4qmail.o conf_qmail.o \
libsys.so conf_qmail_hyp.o vq_init.o cfsvq.o \
cdaemonauth.o libfd.so libtext.so libfs.so
	./load achk4qmail conf_qmail.o \
	conf_qmail_hyp.o `cat pgsql.lib` -lfd -ltext -lfs -lsys

str2cc: str2cc.o load
	./load str2cc

int2cc: int2cc.o load
	./load int2cc

choose: choose.o load
	./load choose

daemon.o: compile daemon.cc daemon.h
	./compile daemon.cc

deliver: load deliver.o conf_qmail.o libsplit.so 
	./load deliver conf_qmail.o -lsplit

dom_add: load dom_add.o libvq.so libsplit.so 
	./load dom_add -lvq -lsplit

dom_alias_add: load dom_alias_add.o libvq.so libsplit.so
	./load dom_alias_add -lvq -lsplit

dom_alias_rm: load dom_alias_rm.o libvq.so libsplit.so 
	./load dom_alias_rm -lvq -lsplit

dom_ip_add: load dom_ip_add.o libvq.so libsplit.so
	./load dom_ip_add -lvq -lsplit

dom_ip_ls: load dom_ip_ls.o libvq.so libsplit.so
	./load dom_ip_ls -lvq -lsplit

dom_ip_rm: load dom_ip_rm.o libvq.so libsplit.so
	./load dom_ip_rm -lvq -lsplit

dom_rm: load dom_rm.o libvq.so libsplit.so
	./load dom_rm -lvq -lsplit

getgid: load getgid.o
	./load getgid

getuid: load getuid.o
	./load getuid

install: load install.o conf_vq.o conf_vq_uid.o conf_vq_gid.o
	./load install conf_vq.o conf_vq_uid.o conf_vq_gid.o

pgsqlauthd: load pgsql.lib pgsqlauthd.o conf_vq.o conf_pgsql.o \
	pgsql.lib pgsqlcommon.o conf_qmail_hyp.o libfd.so libsys.so libfs.so \
	daemon.o
	./load pgsqlauthd conf_vq.o conf_pgsql.o daemon.o \
	pgsqlcommon.o conf_qmail_hyp.o `cat pgsql.lib` -lfd -lsys -lfs

pgsqllogd: load pgsql.lib pgsqllogd.o conf_vq.o conf_pgsql.o sig.o \
	pgsql.lib pgsqlcommon.o libfd.so libsys.so libfs.so daemon.o
	./load pgsqllogd conf_vq.o conf_pgsql.o daemon.o \
	pgsqlcommon.o `cat pgsql.lib` -lfd -lsys -lfs

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

user_auth: load user_auth.o conf_atchars.o libvq.so libfd.so libsys.so \
	libsplit.so hmac_md5.o lmd5.o ccurlog.o ccurlog.lib libtext.so
	./load user_auth conf_atchars.o -lvq -lsys -lfd -ltext -lfs -lsplit \
	hmac_md5.o lmd5.o \
	ccurlog.o -ltext `cat ccurlog.lib` 

user_pass: load user_pass.o libvq.so libsplit.so
	./load user_pass -lvq -lsys -lfd -ltext -lfs -lsplit

user_rm: load user_rm.o libvq.so libsplit.so
	./load user_rm -lvq -lsys -lfd -ltext -lfs -lsplit

######################
achk4qmail.o: compile conf_qmail.h \
sig.h fd.h fdstr.h util.h lock.h achk4qmail.cc \
conf_qmail_hyp.h vq_init.h cvq.h cauth.h sys.h
	./compile achk4qmail.cc `cat pgsql.inc`
	
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

cfsvq.o: comp-so cfsvq.cc cfsvq.h cvq.h lower.h conf_qmail.h lock.h \
	conf_qmail_mode.h dirs.h conf_vq.h conf_vq_mode.h \
	conf_vq_fmode.h split.h cauth.h \
	skillall.h conf_vq_uid.h conf_vq_gid.h conf_vq_user.h d_namlen.h sys.h
	./comp-so cfsvq.cc

choose.o: compile choose.cc
	./compile choose.cc

conf_atchars.cc: str2cc conf/atchars
	./str2cc "`head -n 1 conf/atchars`" atchars a > conf_atchars.cc

conf_atchars.o: comp-so conf_atchars.cc conf_atchars.h
	./comp-so conf_atchars.cc

conf_qmail.cc: str2cc conf/qmail
	./str2cc "`head -n 1 conf/qmail`" qmail a > conf_qmail.cc

conf_qmail.o: comp-so conf_qmail.cc conf_qmail.h
	./comp-so conf_qmail.cc

conf_qmail_hyp.cc: str2cc conf/qmail_hyp
	./str2cc "`head -n 1 conf/qmail_hyp`" qmail_hyp a > conf_qmail_hyp.cc

conf_qmail_hyp.o: comp-so conf_qmail_hyp.h conf_qmail_hyp.cc
	./comp-so conf_qmail_hyp.cc

conf_qmail_mode.cc: int2cc conf/qmail_mode
	./int2cc "`head -n 1 conf/qmail_mode`" qmail_mode > conf_qmail_mode.cc

conf_qmail_mode.o: comp-so conf_qmail_mode.cc conf_qmail_mode.h
	./comp-so conf_qmail_mode.cc

conf_pgsql.cc: str2cc conf/pgsql
	./str2cc "`head -n 1 conf/pgsql`" pgsql a > conf_pgsql.cc

conf_pgsql.o: comp-so conf_pgsql.h conf_pgsql.cc
	./comp-so conf_pgsql.cc

conf_ps.cc: str2cc conf/ps
	./str2cc "`head -n 1 conf/ps`" ps a > conf_ps.cc

conf_ps.o: comp-so conf_ps.h conf_ps.cc
	./comp-so conf_ps.cc
	
conf_split_dom.cc: int2cc conf/split_dom
	./int2cc "`head -n 1 conf/split_dom`" split_dom > conf_split_dom.cc

conf_split_dom.o: comp-so conf_split_dom.cc conf_split_dom.h
	./comp-so conf_split_dom.cc

conf_split_user.cc: int2cc conf/split_user
	./int2cc "`head -n 1 conf/split_user`" split_user > conf_split_user.cc

conf_split_user.o: comp-so conf_split_user.cc conf_split_user.h
	./comp-so conf_split_user.cc

conf_vq.cc: str2cc conf/vq
	./str2cc "`head -n 1 conf/vq`" vq a > conf_vq.cc

conf_vq.o: comp-so conf_vq.cc conf_vq.h
	./comp-so conf_vq.cc

conf_vq_fmode.cc: int2cc conf/vq_fmode
	./int2cc "`head -n 1 conf/vq_fmode`" vq_fmode > conf_vq_fmode.cc
	
conf_vq_fmode.o: comp-so conf_vq_fmode.cc conf_vq_fmode.h
	./comp-so conf_vq_fmode.cc

conf_vq_gid.cc: getgid int2cc conf/vq_user
	./getgid "`head -n 1 conf/vq_user`" vq_gid > conf_vq_gid.cc.tmp \
	&& mv conf_vq_gid.cc.tmp conf_vq_gid.cc
	
conf_vq_gid.o: comp-so conf_vq_gid.cc conf_vq_gid.h
	./comp-so conf_vq_gid.cc

conf_vq_mode.cc: int2cc conf/vq_mode
	./int2cc "`head -n 1 conf/vq_mode`" vq_mode > conf_vq_mode.cc
	
conf_vq_mode.o: comp-so conf_vq_mode.cc conf_vq_mode.h
	./comp-so conf_vq_mode.cc

conf_vq_uid.cc: getuid int2cc conf/vq_user
	./getuid "`head -n 1 conf/vq_user`" vq_uid > conf_vq_uid.cc.tmp \
	&& mv conf_vq_uid.cc.tmp conf_vq_uid.cc

conf_vq_uid.o: comp-so conf_vq_uid.cc conf_vq_uid.h
	./comp-so conf_vq_uid.cc

conf_vq_user.cc: str2cc conf/vq_user
	./str2cc "`head -n 1 conf/vq_user`" vq_user a > conf_vq_user.cc

conf_vq_user.o: comp-so conf_vq_user.cc conf_vq_user.h
	./comp-so conf_vq_user.cc

cpoll.o: comp-so sys.h cpoll.cc
	./comp-so cpoll.cc

csyslog.o: compile csyslog.cc csyslog.h clog.h 
	./compile csyslog.cc

deliver.o: compile deliver.cc conf_qmail.h split.h vq_init.h cvq.h cauth.h
	./compile deliver.cc

dirs.o: comp-so dirs.cc dirs.h
	./comp-so dirs.cc

d_namlen.h: compile tryd_namlen.cc d_namlen.h1 d_namlen.h2
	( ./compile tryd_namlen.cc >/dev/null 2>&1 \
	&& cat d_namlen.h1 || cat d_namlen.h2 ) > d_namlen.h
	rm -f tryd_namlen.o 

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

getline.o: compile getline.h getline.cc
	./compile getline.cc

getuid.o: compile getuid.cc
	./compile getuid.cc

hasflock.h: tryflock.cc compile load
	( ( ./compile tryflock.cc && ./load tryflock ) >/dev/null 2>&1 \
    && echo \#define HASFLOCK 1 || exit 0 ) > hasflock.h
	rm -f tryflock.o tryflock

hex.o: comp-so hex.cc sys.h
	./comp-so hex.cc

hmac_md5.o: compile hmac_md5.h lmd5.h
	./compile hmac_md5.c

install.o: compile install.cc conf_vq.h conf_vq_uid.h conf_vq_gid.h
	./compile install.cc
	
int2cc.o: compile int2cc.cc
	./compile int2cc.cc

libfd.so: libfd.so.1
	ln -sf libfd.so.1 libfd.so

libfd.so.1: make-so fd.o fdstr.o getline.o
	./make-so libfd.so.1 fd.o fdstr.o getline.o

libvq.so: libvq.so.1
	ln -sf libvq.so.1 libvq.so

libvq.so.1: make-so cfsvq.o conf_qmail.o conf_qmail_mode.o \
	conf_vq.o conf_vq_mode.o conf_vq_fmode.o \
	conf_vq_uid.o conf_vq_gid.o \
	conf_vq_user.o cdaemonauth.o libfd.so libtext.so libsys.so \
	libfs.so vq_init.o
	./make-so libvq.so.1 cfsvq.o conf_qmail.o conf_qmail_mode.o \
	conf_vq.o conf_vq_mode.o conf_vq_fmode.o \
	conf_vq_uid.o conf_vq_gid.o \
	conf_vq_user.o cdaemonauth.o vq_init.o -lfd -ltext \
	-lsys -lfs

libsplit.so: libsplit.so.1
	ln -sf libsplit.so.1 libsplit.so

libsplit.so.1: make-so split.o conf_split_dom.o conf_split_user.o
	./make-so libsplit.so.1 split.o conf_split_dom.o conf_split_user.o

libsys.so: libsys.so.1
	ln -sf libsys.so.1 libsys.so

libsys.so.1: make-so skillall.o sig.o cpoll.o hex.o conf_ps.o
	./make-so libsys.so.1 skillall.o sig.o cpoll.o hex.o conf_ps.o

libfs.so: libfs.so.1
	ln -sf libfs.so.1 libfs.so

libfs.so.1: make-so util.o lock.o dirs.o
	./make-so libfs.so.1 util.o dirs.o lock.o

libtext.so: libtext.so.1
	ln -sf libtext.so.1 libtext.so

libtext.so.1: make-so lower.o
	./make-so libtext.so.1 lower.o

lock.o: comp-so lock.cc lock.h hasflock.h
	./comp-so lock.cc

lower.o: comp-so lower.cc lower.h
	./comp-so lower.cc

lmd5.c: compile trymd5.c md5.c1 md5.c2
	( ./compile trymd5.c >/dev/null 2>&1 \
	&& cat md5.c1 || cat md5.c2 ) > lmd5.c
	rm -f trymd5.o 

lmd5.h: compile trymd5.c md5.h1 md5.h2
	( ./compile trymd5.c >/dev/null 2>&1 \
	&& cat md5.h1 || cat md5.h2 ) > lmd5.h
	rm -f trymd5.o

lmd5.o: compile lmd5.c lmd5.h
	./compile lmd5.c

pgsqlauthd.o: compile cdaemonauth.h pgsql.inc conf_vq.h conf_pgsql.h \
	sig.h fd.h fdstr.h pgsql.inc util.h lock.h pgsqlauthd.cc pgsqlcommon.h \
	conf_qmail_hyp.h sys.h daemon.h
	./compile pgsqlauthd.cc `cat pgsql.inc`

pgsqlcommon.o: compile pgsqlcommon.h pgsqlcommon.cc pgsql.inc util.h
	./compile pgsqlcommon.cc `cat pgsql.inc`

pgsqllogd.o: compile cdaemonlog.h pgsql.inc conf_vq.h conf_pgsql.h \
sig.h fd.h fdstr.h pgsql.inc util.h lock.h pgsqllogd.cc pgsqlcommon.h \
sys.h	daemon.h
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

skillall.o: comp-so conf_ps.h skillall.cc getline.h
	./comp-so skillall.cc

split.o: comp-so split.h conf_split_dom.h conf_split_user.h split.cc
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

user_auth.o: compile user_auth.cc cvq.h cauth.h conf_atchars.h \
	lmd5.h ccurlog.h ccurlog.inc vq_init.h sig.h sys.h
	./compile user_auth.cc `cat ccurlog.inc`

user_pass.o: compile user_pass.cc cvq.h cfsvq.h cdaemonauth.h vq_init.h
	./compile user_pass.cc

user_rm.o: compile user_add.cc cvq.h cfsvq.h cdaemonauth.h vq_init.h
	./compile user_rm.cc

util.o: comp-so util.cc util.h
	./comp-so util.cc

vq_init.o: comp-so vq_init.h vq_init.cc
	./comp-so vq_init.cc

#####################
clean:
		rm -rf `cat TARGETS`
