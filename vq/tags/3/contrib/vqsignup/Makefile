all: auto vqsignup install

auto:
	mkdir auto
	
#######################
cgi.o: compile cgi.cc cgi.h html.h cgicc.inc
	./compile cgi.cc `cat cgicc.inc`

ac_int: load ac_int.o conf_home.o
	./load ac_int conf_home.o

ac_int.o: compile ac_int.cc conf_home.h
	./compile ac_int.cc

ac_str: load ac_str.o conf_home.o
	./load ac_str conf_home.o

ac_str.o: compile ac_str.cc conf_home.h
	./compile ac_str.cc

auto/domains.cc: ac_str conf/domains
	head -n 1 conf/domains | ./ac_str vqsignup domains a

auto/domains.o: compile auto/domains.cc cconf.h
	cd auto && ../compile domains.cc 

auto/html_br.cc: ac_str conf/html_br
	head -n 1 conf/html_br | ./ac_str vqsignup html_br a

auto/html_br.o: compile cconf.h auto/html_br.cc
	cd auto && ../compile html_br.cc

auto/iconv.lib: try/iconv.c compile load
	( cd try && ../compile iconv.c && ../load iconv >/dev/null 2>&1 \
	&& echo "" || echo -liconv ) > auto/iconv.lib
	rm -f try/iconv.o try/iconv

auto/pass_ok.cc: ac_str conf/pass_ok
	head -n 1 conf/pass_ok | ./ac_str vqsignup pass_ok a

auto/pass_ok.o: compile auto/pass_ok.cc cconf.h
	cd auto && ../compile pass_ok.cc

auto/user_ok.cc: ac_str conf/user_ok
	head -n 1 conf/user_ok | ./ac_str vqsignup user_ok a
	
auto/user_ok.o: compile auto/user_ok.cc cconf.h
	cd auto && ../compile user_ok.cc

auto/vq_gid.cc: getgid ac_int conf/vq_user
	./getgid "`head -n 1 conf/vq_user`" | ./ac_int vqsignup vq_gid 1 cgidconf
	
auto/vq_gid.o: compile auto/vq_gid.cc cconf.h
	cd auto && ../compile vq_gid.cc

auto/vq_uid.cc: getuid ac_int conf/vq_user
	./getuid "`head -n 1 conf/vq_user`" | ./ac_int vqsignup vq_uid 1 cuidconf

auto/vq_uid.o: compile auto/vq_uid.cc cconf.h
	cd auto && ../compile vq_uid.cc

auto/xmlstart.cc: ac_str conf/xmlstart
	head -n 1 conf/xmlstart | ./ac_str vqsignup xmlstart

auto/xmlstart.o: compile auto/xmlstart.cc cconf.h
	cd auto && ../compile xmlstart.cc

cconf.o: compile cconf.h cconf.cc
	./compile cconf.cc

conf_home.cc: str2cc conf/home
	./str2cc vqsignup "`head -n 1 conf/home`" home a > conf_home.cc.tmp && \
	mv conf_home.cc.tmp conf_home.cc

conf_home.o: compile conf_home.cc conf_home.h
	./compile conf_home.cc
	
getgid: load getgid.o
	./load getgid

getgid.o: compile getgid.cc
	./compile getgid.cc

getuid: load getuid.o
	./load getuid

getuid.o: compile getuid.cc
	./compile getuid.cc

hier.o: hier.cc compile install.h conf_home.h cconf.h vqsignup_conf.h
	./compile hier.cc

html.o: compile html.cc html.h
	./compile html.cc

install: load install.o hier.o conf_home.o vqsignup_conf.a
	./load install hier.o vqsignup_conf.a conf_home.o
	
install.o: compile install.cc
	./compile install.cc

sabmain.o: compile sabmain.cc conf_home.h sab.inc
	./compile sabmain.cc `cat sab.inc`

str2cc: str2cc.o load
	./load str2cc

str2cc.o: compile str2cc.cc
	./compile str2cc.cc

vqsignup: load vqsignup.o sabmain.o sab.lib cgi.o cgicc.lib \
html.o vq.lib conf_home.o vqsignup_conf.a auto/iconv.lib
	./load vqsignup sabmain.o `cat sab.lib` cgi.o `cat cgicc.lib` \
	html.o conf_home.o vqsignup_conf.a `cat vq.lib` `cat auto/iconv.lib`

vqsignup.o: compile vqsignup.cc cgicc.inc html.h cconf.h vqsignup_conf.h vq.inc
	./compile vqsignup.cc `cat cgicc.inc vq.inc`

vqsignup_conf.a: make-lib auto/domains.o auto/html_br.o auto/pass_ok.o \
auto/user_ok.o auto/vq_gid.o auto/vq_uid.o auto/xmlstart.o cconf.o
	./make-lib vqsignup_conf.a auto/domains.o auto/html_br.o auto/pass_ok.o \
	auto/user_ok.o auto/vq_gid.o auto/vq_uid.o auto/xmlstart.o cconf.o

#####################
clean:
		rm -rf `cat TARGETS`
