.PHONY: auto-mkdir

all: auto-mkdir vqsignup1 vqsignup1_notify install

setup: all
	./install

#######################
ac_int: load ac_int.o conf_home.o
	./load ac_int conf_home.o

ac_int.cc: ../../ac_int.cc
	cp -f ../../ac_int.cc .

ac_int.o: compile ac_int.cc conf_home.h
	./compile ac_int.cc -DHOME_NS=vqsignup1

ac_str: load ac_str.o conf_home.o
	./load ac_str conf_home.o

ac_str.cc: ../../ac_str.cc
	cp -f ../../ac_str.cc .
	
ac_str.o: compile ac_str.cc conf_home.h
	./compile ac_str.cc -DHOME_NS=vqsignup1

auto-mkdir:
	mkdir -p auto

cconf.cc: ../../cconf.cc
	cp -f ../../cconf.cc .

cconf.h: ../../cconf.h
	cp -f ../../cconf.h .

cconf.o: compile cconf.cc cconf.h
	./compile cconf.cc

cgi.o: compile cgi.cc cgi.h html.h cgicc.inc
	./compile cgi.cc `cat cgicc.inc`

conf_home.cc: str2cc conf/home
	./str2cc vqsignup1 "`head -n 1 conf/home`" home a > conf_home.cc

conf_home.o: compile conf_home.cc conf_home.h
	./compile conf_home.cc

auto/domains.cc: ac_str conf/domains
	head -n 1 conf/domains | ./ac_str vqsignup1 domains a

auto/domains.o: compile auto/domains.cc cconf.h
	cd auto && ../compile domains.cc 

auto/html_br.cc: ac_str conf/html_br
	head -n 1 conf/html_br | ./ac_str vqsignup1 html_br a 

auto/html_br.o: compile cconf.h auto/html_br.cc
	cd auto && ../compile html_br.cc

auto/notify.cc: ac_str conf/notify
	head -n 1 conf/notify | ./ac_str vqsignup1 notify a

auto/notify.o: compile cconf.h auto/notify.cc
	cd auto && ../compile notify.cc
	
auto/pass_ok.cc: ac_str conf/pass_ok
	head -n 1 conf/pass_ok | ./ac_str vqsignup1 pass_ok a

auto/pass_ok.o: compile auto/pass_ok.cc cconf.h
	cd auto && ../compile pass_ok.cc

auto/pgsql.cc: ac_str conf/pgsql
	head -n 1 conf/pgsql | ./ac_str vqsignup1 pgsql a

auto/pgsql.o: compile cconf.h auto/pgsql.cc
	cd auto && ../compile pgsql.cc

auto/user_ok.cc: ac_str conf/user_ok
	head -n 1 conf/user_ok | ./ac_str vqsignup1 user_ok a
	
auto/user_ok.o: compile auto/user_ok.cc cconf.h
	cd auto && ../compile user_ok.cc

auto/vq_gid.cc: getgid ac_int conf/vq_user
	./getgid "`head -n 1 conf/vq_user`" | ./ac_int vqsignup1 vq_gid 1 cgidconf
	
auto/vq_gid.o: compile auto/vq_gid.cc cconf.h
	cd auto && ../compile vq_gid.cc

auto/vq_uid.cc: getuid ac_int conf/vq_user
	./getuid "`head -n 1 conf/vq_user`" | ./ac_int vqsignup1 vq_uid 1 cuidconf

auto/vq_uid.o: compile auto/vq_uid.cc cconf.h
	cd auto && ../compile vq_uid.cc

auto/xmlstart.cc: ac_str conf/xmlstart
	head -n 1 conf/xmlstart | ./ac_str vqsignup1 xmlstart a

auto/xmlstart.o: compile auto/xmlstart.cc cconf.h
	cd auto && ../compile xmlstart.cc

auto/xmlstart1.cc: ac_str conf/xmlstart1
	head -n 1 conf/xmlstart1 | ./ac_str vqsignup1 xmlstart1 a

auto/xmlstart1.o: compile auto/xmlstart1.cc cconf.h
	cd auto && ../compile xmlstart1.cc

getgid: load getgid.o
	./load getgid

getgid.cc: ../../getgid.cc
	cp -f ../../getgid.cc .
	
getgid.o: compile getgid.cc
	./compile getgid.cc

getuid: load getuid.o
	./load getuid

getuid.cc: ../../getuid.cc
	cp -f ../../getuid.cc .

getuid.o: compile getuid.cc
	./compile getuid.cc

hier.o: compile hier.cc install.h conf_home.h vqsignup1_conf.h
	./compile hier.cc

html.o: compile html.cc html.h
	./compile html.cc

install.cc: ../../install.cc
	cp -f ../../install.cc .

install.h: ../../install.h
	cp -f ../../install.h .

install: load install.o hier.o vqsignup1_conf.a conf_home.o
	./load install hier.o vqsignup1_conf.a conf_home.o
	
install.o: compile install.cc install.h
	./compile install.cc

sabmain.o: compile sabmain.cc vqsignup1_conf.h sab.inc
	./compile sabmain.cc `cat sab.inc`

str2cc: str2cc.o load
	./load str2cc

str2cc.cc: ../../str2cc.cc
	cp -f ../../str2cc.cc .

str2cc.o: compile str2cc.cc
	./compile str2cc.cc

vqsignup1: load vqsignup1.o sabmain.o sab.lib cgi.o cgicc.lib \
html.o vq.lib valid.lib pgsql.lib vqsignup1_conf.a iconv.lib conf_home.o
	./load vqsignup1 sabmain.o `cat sab.lib` cgi.o `cat cgicc.lib` \
	html.o vqsignup1_conf.a `cat vq.lib valid.lib pgsql.lib` \
	`cat iconv.lib` conf_home.o

vqsignup1.o: compile vqsignup1.cc cgicc.inc html.h vq.inc valid.inc pgsql.inc \
vqsignup1_conf.h cconf.h sabmain.h
	./compile vqsignup1.cc `cat cgicc.inc vq.inc valid.inc pgsql.inc`

vqsignup1_conf.a: make-lib auto/domains.o auto/html_br.o auto/notify.o \
auto/pass_ok.o auto/pgsql.o auto/user_ok.o auto/vq_gid.o auto/vq_uid.o \
auto/xmlstart.o auto/xmlstart1.o cconf.o
	./make-lib vqsignup1_conf.a auto/domains.o auto/html_br.o auto/notify.o \
	auto/pass_ok.o auto/pgsql.o auto/user_ok.o auto/vq_gid.o auto/vq_uid.o \
	auto/xmlstart.o auto/xmlstart1.o cconf.o

vqsignup1_notify: load vqsignup1_notify.o sab.lib html.o pgsql.lib vq.lib \
vqsignup1_conf.a conf_home.o iconv.lib
	./load vqsignup1_notify vqsignup1_conf.a html.o \
	`cat sab.lib pgsql.lib vq.lib` `cat iconv.lib` conf_home.o

vqsignup1_notify.o: compile vqsignup1_notify.cc html.h sab.inc pgsql.inc \
vq.inc vqsignup1_conf.h conf_home.h
	./compile vqsignup1_notify.cc `cat sab.inc pgsql.inc vq.inc`

#####################
clean:
		rm -rf `cat TARGETS`
