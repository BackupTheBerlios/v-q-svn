all: auto vquserrm install

clean:
	rm -rf `cat TARGETS`

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

cgi.o: cgi.cc cgicc.inc
	./compile cgi.cc `cat cgicc.inc`

cconf.o: compile cconf.cc cconf.h
	./compile cconf.cc

conf_home.cc: str2cc conf/home
	./str2cc vquserrm "`head -n 1 conf/home`" home a > conf_home.cc.tmp && \
	mv conf_home.cc.tmp conf_home.cc

conf_home.o: compile conf_home.cc conf_home.h
	./compile conf_home.cc

auto/vq_gid.cc: getgid ac_int conf/vq_user
	./getgid "`head -n 1 conf/vq_user`" | ./ac_int vquserrm vq_gid 1 cgidconf

auto/vq_gid.o: compile auto/vq_gid.cc cconf.h
	cd auto && ../compile vq_gid.cc

auto/vq_uid.cc: getuid ac_int conf/vq_user
	./getuid "`head -n 1 conf/vq_user`" | ./ac_int vquserrm vq_uid 1 cuidconf

auto/vq_uid.o: compile auto/vq_uid.cc cconf.h
	cd auto && ../compile vq_uid.cc 

auto/xmlstart.cc: ac_str conf/xmlstart
	head -n 1 conf/xmlstart | ./ac_str vquserrm xmlstart

auto/xmlstart.o: compile auto/xmlstart.cc cconf.h
	cd auto && ../compile xmlstart.cc

getgid: getgid.o load
	./load getgid

getgid.o: compile getgid.cc
	./compile getgid.cc

getuid: getuid.o load
	./load getuid

getuid.o: compile getuid.cc
	./compile getuid.cc

hier.o: hier.cc vquserrm_conf.h cconf.h install.h
	./compile hier.cc

html.o: compile html.cc 
	./compile html.cc

install: load install.o hier.o vquserrm_conf.a conf_home.o
	./load install hier.o vquserrm_conf.a conf_home.o

install.o: compile install.cc install.h
	./compile install.cc
	
sabmain.o: compile sabmain.cc vquserrm_conf.h conf_home.h sab.inc
	./compile sabmain.cc `cat sab.inc`
 
str2cc: str2cc.o load
	./load str2cc

str2cc.o: compile str2cc.cc
	./compile str2cc.cc  

vquserrm: load vquserrm.o vq.lib sabmain.o sab.lib \
cgicc.lib cgi.o html.o vquserrm_conf.a conf_home.o auto/iconv.lib
	./load vquserrm sabmain.o cgi.o html.o vquserrm_conf.a \
	conf_home.o `cat vq.lib sab.lib cgicc.lib` `cat auto/iconv.lib`

vquserrm.o: compile vquserrm.cc vq.inc cgicc.inc cgi.h html.h vquserrm_conf.h
	./compile vquserrm.cc `cat vq.inc` `cat cgicc.inc`

vquserrm_conf.a: make-lib cconf.o auto/vq_gid.o auto/vq_uid.o auto/xmlstart.o
	./make-lib vquserrm_conf.a auto/vq_gid.o auto/vq_uid.o auto/xmlstart.o \
	cconf.o

