all: auto vqsignup1 install vqsignup1d vqrescue libfreemail.so

setup: all
	./install

#######################
auto:
	mkdir auto

ac_int: load ac_int.o conf_home.o
	./load ac_int conf_home.o

ac_int.o: compile ac_int.cc conf_home.h
	./compile ac_int.cc

ac_str: load ac_str.o conf_home.o
	./load ac_str conf_home.o

ac_str.o: compile ac_str.cc conf_home.h
	./compile ac_str.cc

auto/iconv.lib: try/iconv.c compile load
	( cd try && ../compile iconv.c && ../load iconv >/dev/null 2>&1 \
	&& echo "" || echo -liconv ) > auto/iconv.lib
	rm -f try/iconv.o try/iconv

cdbdaemon.o: comp-so cdbdaemon.cc vq.inc conf_home.h
	./comp-so cdbdaemon.cc `cat vq.inc`

cgi.o: compile cgi.cc cgi.h html.h cgicc.inc
	./compile cgi.cc `cat cgicc.inc`

auto/city_minl.cc: ac_int conf/city_minl 
	head -n 1 conf/city_minl | ./ac_int freemail city_minl

auto/city_minl.o: comp-so auto/city_minl.cc cconf.h
	cd auto && ../comp-so city_minl.cc

auto/cntry_minl.cc: ac_int conf/cntry_minl 
	head -n 1 conf/cntry_minl | ./ac_int freemail cntry_minl

auto/cntry_minl.o: comp-so auto/cntry_minl.cc cconf.h
	cd auto && ../comp-so cntry_minl.cc

conf_home.cc: str2cc conf/home
	./str2cc freemail "`head -n 1 conf/home`" home a > conf_home.cc

conf_home.o: comp-so conf_home.cc conf_home.h
	./comp-so conf_home.cc

auto/html_br.cc: ac_str conf/html_br
	head -n 1 conf/html_br | ./ac_str freemail html_br a

auto/html_br.o: comp-so auto/html_br.cc cconf.h
	cd auto && ../comp-so html_br.cc

auto/notify.cc: ac_str conf/notify
	head -n 1 conf/notify | ./ac_str freemail notify a

auto/notify.o: comp-so auto/notify.cc cconf.h
	cd auto && ../comp-so notify.cc

auto/pass_maxl.cc: ac_int conf/pass_maxl
	head -n 1 conf/pass_maxl | ./ac_int freemail pass_maxl

auto/pass_maxl.o: comp-so auto/pass_maxl.cc cconf.h
	cd auto && ../comp-so pass_maxl.cc

auto/pass_minl.cc: ac_int conf/pass_minl
	head -n 1 conf/pass_minl | ./ac_int freemail pass_minl

auto/pass_minl.o: comp-so auto/pass_minl.cc cconf.h
	cd auto && ../comp-so pass_minl.cc

auto/pass_ok.cc: ac_str conf/pass_ok
	head -n 1 conf/pass_ok | ./ac_str freemail pass_ok

auto/pass_ok.o: comp-so auto/pass_ok.cc cconf.h
	cd auto && ../comp-so pass_ok.cc

auto/pgsql.cc: ac_str conf/pgsql
	head -n 1 conf/pgsql | ./ac_str freemail pgsql a

auto/pgsql.o: comp-so auto/pgsql.cc cconf.h
	cd auto && ../comp-so pgsql.cc

auto/user_maxl.cc: ac_int conf/user_maxl
	head -n 1 conf/user_maxl | ./ac_int freemail user_maxl

auto/user_maxl.o: comp-so auto/user_maxl.cc cconf.h
	cd auto && ../comp-so user_maxl.cc

auto/user_minl.cc: ac_int conf/user_minl
	head -n 1 conf/user_minl | ./ac_int freemail user_minl

auto/user_minl.o: comp-so auto/user_minl.cc cconf.h
	cd auto && ../comp-so user_minl.cc

auto/user_ok.cc: ac_str conf/user_ok
	head -n 1 conf/user_ok | ./ac_str freemail user_ok a
	
auto/user_ok.o: comp-so auto/user_ok.cc cconf.h
	cd auto && ../comp-so user_ok.cc

auto/vq_gid.cc: getgid ac_int conf/vq_user
	./getgid "`head -n 1 conf/vq_user`" | ./ac_int freemail vq_gid 1 cgidconf
	
auto/vq_gid.o: comp-so auto/vq_gid.cc cconf.h
	cd auto && ../comp-so vq_gid.cc

auto/vq_uid.cc: getuid ac_int conf/vq_user
	./getuid "`head -n 1 conf/vq_user`" | ./ac_int freemail vq_uid 1 cuidconf

auto/vq_uid.o: comp-so auto/vq_uid.cc cconf.h
	cd auto && ../comp-so vq_uid.cc

auto/xmlstart.cc: ac_str conf/xmlstart
	head -n 1 conf/xmlstart | ./ac_str freemail xmlstart

auto/xmlstart.o: comp-so auto/xmlstart.cc cconf.h
	cd auto && ../comp-so xmlstart.cc

cconf.o: comp-so cconf.cc cconf.h
	./comp-so cconf.cc
	
cuimod.o: comp-so cuimod.h cuimod.h vqwww.inc cgicc.inc vq.inc \
freemail_conf.h cconf.h valid.inc
	./comp-so cuimod.cc `cat vqwww.inc` `cat cgicc.inc` `cat vq.inc` \
	`cat valid.inc`

daemon.o: compile daemon.cc daemon.h vq.inc
	./compile daemon.cc `cat vq.inc`

freemail.o: comp-so freemail.cc cuimod.h vqwww.inc vq.inc cgicc.inc
	./comp-so freemail.cc `cat vqwww.inc` `cat vq.inc` `cat cgicc.inc`

freemail_conf.a: make-lib auto/city_minl.o auto/cntry_minl.o \
auto/html_br.o auto/notify.o auto/pass_maxl.o auto/pass_minl.o auto/pass_ok.o \
auto/pgsql.o auto/user_maxl.o auto/user_minl.o auto/user_ok.o  \
auto/vq_gid.o auto/vq_uid.o auto/xmlstart.o cconf.o
	./make-lib freemail_conf.a auto/city_minl.o auto/cntry_minl.o \
	auto/html_br.o auto/notify.o auto/pass_maxl.o auto/pass_minl.o \
	auto/pass_ok.o auto/pgsql.o auto/user_maxl.o auto/user_minl.o \
	auto/user_ok.o auto/vq_gid.o auto/vq_uid.o auto/xmlstart.o cconf.o

getgid: load getgid.o
	./load getgid

getgid.o: compile getgid.cc
	./compile getgid.cc

getuid: load getuid.o
	./load getuid

getuid.o: compile getuid.cc
	./compile getuid.cc

hier.o: hier.cc freemail_conf.h cconf.h install.h
	./compile hier.cc

html.o: compile html.cc html.h
	./compile html.cc

install: load install.o hier.o freemail_conf.h cconf.h conf_home.o
	./load install hier.o freemail_conf.a conf_home.o
	
install.o: compile install.cc
	./compile install.cc

int2cc: int2cc.o load
	./load int2cc

int2cc.o: compile int2cc.cc
	./compile int2cc.cc

libfreemail.so: make-so freemail.o cuimod.o cdbdaemon.o pgsql.lib vq.lib \
cgicc.lib valid.lib freemail_conf.a conf_home.o
	./make-so freemail freemail.o cuimod.o cdbdaemon.o \
	freemail_conf.a conf_home.o `cat pgsql.lib` \
	`cat vq.lib` `cat cgicc.lib` `cat valid.lib`

sabmain.o: compile sabmain.cc sab.inc freemail_conf.h cconf.h conf_home.h
	./compile sabmain.cc `cat sab.inc`

str2cc: str2cc.o load
	./load str2cc

str2cc.o: compile str2cc.cc
	./compile str2cc.cc

vqrescue: load vqrescue.o sabmain.o sab.lib cgi.o cgicc.lib \
html.o vq.lib valid.lib pgsql.lib cdbdaemon.o freemail_conf.a \
conf_home.o auto/iconv.lib
	./load vqrescue sabmain.o cgi.o html.o cdbdaemon.o freemail_conf.a \
	conf_home.o \
	`cat vq.lib valid.lib pgsql.lib` `cat sab.lib` `cat cgicc.lib` \
	`cat auto/iconv.lib`

vqrescue.o: compile vqrescue.cc cgicc.inc \
html.h vq.inc valid.inc pgsql.inc cdbdaemon.h freemail_conf.h cconf.h
	./compile vqrescue.cc `cat cgicc.inc vq.inc valid.inc pgsql.inc`

vqsignup1: load vqsignup1.o sabmain.o sab.lib cgi.o cgicc.lib \
html.o vq.lib valid.lib pgsql.lib cdbdaemon.o freemail_conf.a \
conf_home.o auto/iconv.lib
	./load vqsignup1 sabmain.o cgi.o html.o cdbdaemon.o freemail_conf.a \
	conf_home.o \
	`cat vq.lib valid.lib pgsql.lib` `cat sab.lib` `cat cgicc.lib` \
	`cat auto/iconv.lib`

vqsignup1.o: compile vqsignup1.cc cgicc.inc html.h vq.inc valid.inc pgsql.inc \
cdbdaemon.h freemail_conf.h cconf.h
	./compile vqsignup1.cc `cat cgicc.inc vq.inc valid.inc pgsql.inc`

vqsignup1_notify: load vqsignup1_notify.o sab.lib \
html.o pgsql.lib vq.lib freemail_conf.a conf_home.o auto/iconv.lib
	./load vqsignup1_notify html.o freemail_conf.a conf_home.o \
	`cat sab.lib pgsql.lib vq.lib` `cat auto/iconv.lib`

vqsignup1_notify.o: compile vqsignup1_notify.cc html.h sab.inc pgsql.inc \
vq.inc freemail_conf.h cconf.h
	./compile vqsignup1_notify.cc `cat sab.inc pgsql.inc vq.inc`

vqsignup1d: load vqsignup1d.o vq.lib pgsql.lib daemon.o freemail_conf.a \
conf_home.o
	./load vqsignup1d daemon.o freemail_conf.a conf_home.o \
	`cat vq.lib` `cat pgsql.lib`

vqsignup1d.o: compile vqsignup1d.cc vq.inc pgsql.inc daemon.h \
freemail_conf.h cconf.h
	./compile vqsignup1d.cc `cat vq.inc` `cat pgsql.inc`

#####################
clean:
		rm -rf `cat TARGETS`
