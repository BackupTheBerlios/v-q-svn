.PHONY: auto-mkdir

all: auto-mkdir host_add install

#######################
ac_int: load ac_int.o conf_home.o
	./load ac_int conf_home.o

ac_int.cc: ../../ac_int.cc
	cp -f ../../ac_int.cc .

ac_int.o: compile ac_int.cc conf_home.h
	./compile ac_int.cc -DHOME_NS=host_add

ac_str: load ac_str.o conf_home.o
	./load ac_str conf_home.o

ac_str.cc: ../../ac_str.cc
	cp -f ../../ac_str.cc .

ac_str.o: compile ac_str.cc conf_home.h
	./compile ac_str.cc -DHOME_NS=host_add

auto-mkdir:
	mkdir -p auto

cgi.o: compile cgi.cc cgi.h html.h cgicc.inc
	./compile cgi.cc `cat cgicc.inc`

auto/domains.cc: ac_str conf/domains
	head -n 1 conf/domains | ./ac_str host_add domains a

auto/domains.o: compile cconf.h auto/domains.cc
	cd auto && ../compile domains.cc 

conf_home.cc: str2cc conf/home
	./str2cc host_add "`head -n 1 conf/home`" home a > conf_home.cc.tmp && \
	mv conf_home.cc.tmp conf_home.cc

conf_home.o: compile conf_home.cc
	./compile conf_home.cc

auto/html_br.cc: ac_str conf/html_br
	head -n 1 conf/html_br | ./ac_str host_add html_br a

auto/html_br.o: compile cconf.h auto/html_br.cc
	cd auto && ../compile html_br.cc

auto/pass_ok.cc: ac_str conf/pass_ok
	head -n 1 conf/pass_ok | ./ac_str host_add pass_ok a

auto/pass_ok.o: compile auto/pass_ok.cc cconf.h
	cd auto && ../compile pass_ok.cc
	
auto/pref_ok.cc: ac_str conf/pref_ok
	head -n 1 conf/pref_ok | ./ac_str host_add pref_ok a

auto/pref_ok.o: compile auto/pref_ok.cc cconf.h
	cd auto && ../compile pref_ok.cc
	
auto/user_ok.cc: ac_str conf/user_ok
	head -n 1 conf/user_ok | ./ac_str host_add user_ok a

auto/user_ok.o: compile auto/user_ok.cc cconf.h
	cd auto && ../compile user_ok.cc

auto/vq_gid.cc: getgid ac_int conf/vq_user
	./getgid "`head -n 1 conf/vq_user`" | ./ac_int host_add vq_gid 1 cgidconf
	
auto/vq_gid.o: compile auto/vq_gid.cc cconf.h
	cd auto && ../compile vq_gid.cc

auto/vq_uid.cc: getuid ac_int conf/vq_user
	./getuid "`head -n 1 conf/vq_user`" | ./ac_int host_add vq_uid 1 cuidconf

auto/vq_uid.o: compile auto/vq_uid.cc cconf.h
	cd auto && ../compile vq_uid.cc

auto/xmlstart.cc: ac_str conf/xmlstart
	head -n 1 conf/xmlstart | ./ac_str host_add xmlstart

auto/xmlstart.o: compile auto/xmlstart.cc cconf.h
	cd auto && ../compile xmlstart.cc

cconf.cc: ../../cconf.cc
	cp -f ../../cconf.cc .

cconf.h: ../../cconf.h
	cp -f ../../cconf.h .

cconf.o: compile cconf.cc cconf.h
	./compile cconf.cc

html.o: compile html.cc html.h
	./compile html.cc

host_add_conf.a: make-lib auto/domains.o auto/html_br.o auto/pass_ok.o \
auto/pref_ok.o auto/user_ok.o auto/vq_gid.o auto/vq_uid.o \
auto/xmlstart.o cconf.o
	./make-lib host_add_conf.a auto/domains.o auto/html_br.o auto/pass_ok.o \
	auto/pref_ok.o auto/user_ok.o auto/vq_gid.o auto/vq_uid.o \
	auto/xmlstart.o cconf.o

sabmain.o: compile sabmain.cc host_add_conf.h sab.inc
	./compile sabmain.cc `cat sab.inc`

str2cc: str2cc.o load
	./load str2cc

str2cc.cc: ../../str2cc.cc
	cp -f ../../str2cc.cc .

str2cc.o: compile str2cc.cc
	./compile str2cc.cc

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

hier.o: compile hier.cc install.h host_add_conf.h cconf.h
	./compile hier.cc

install.cc: ../../install.cc
	cp -f ../../install.cc .

install.h: ../../install.h
	cp -f ../../install.h .
		
install: load install.o conf_home.o host_add_conf.a hier.o
	./load install hier.o host_add_conf.a conf_home.o

install.o: compile install.cc install.h
	./compile install.cc

host_add: load host_add.o sabmain.o sab.lib \
cgi.o cgicc.lib html.o host_add_conf.a vq.lib conf_home.o iconv.lib
	./load host_add sabmain.o `cat sab.lib` \
   	cgi.o `cat cgicc.lib` html.o `cat vq.lib` host_add_conf.a conf_home.o \
	`cat iconv.lib`

host_add.o: compile host_add.cc cgicc.inc html.h vq.inc host_add_conf.h \
cconf.h sabmain.h
	./compile host_add.cc `cat cgicc.inc` `cat vq.inc`

#####################
clean:
		rm -rf `cat TARGETS`
