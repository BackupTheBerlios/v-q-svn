.PHONY: auto-mkdir

all: auto-mkdir vqwww install

#######################
ac_gen: load ac_gen.o conf_home.o
	./load ac_gen conf_home.o

ac_gen.o: compile ac_gen.cc conf_home.h
	./compile ac_gen.cc -DHOME_NS=vqwww

ac_int: load ac_int.o conf_home.o
	./load ac_int conf_home.o

ac_int.cc: ../../ac_int.cc
	cp -f ../../ac_int.cc .

ac_int.o: compile ac_int.cc conf_home.h
	./compile ac_int.cc -DHOME_NS=vqwww

ac_str: load ac_str.o conf_home.o
	./load ac_str conf_home.o

ac_str.cc: ../../ac_str.cc
	cp -f ../../ac_str.cc .

ac_str.o: compile ac_str.cc conf_home.h
	./compile ac_str.cc -DHOME_NS=vqwww

auto-mkdir:
	mkdir -p auto

auto/dl.lib: try/dl.c compile load
	( cd try && ../compile dl.c && ../load dl -ldl >/dev/null 2>&1 \
	&& echo -ldl || echo "" ) > auto/dl.lib
	rm -f try/dl.o try/dl

auto/codepages.cc: ac_gen
	./ac_gen vqwww cmapconf codepages 

auto/codepages.o: comp-so auto/codepages.cc cconf.h
	cd auto && ../comp-so codepages.cc

auto/fs.cc: ac_str conf/fs
	head -n 1 conf/fs | ./ac_str vqwww fs a

auto/fs.o: comp-so auto/fs.cc cconf.h
	cd auto && ../comp-so fs.cc

auto/fs_split.cc: ac_int conf/fs_split
	head -n 1 conf/fs_split | ./ac_int vqwww fs_split

auto/fs_split.o: comp-so auto/fs_split.cc cconf.h
	cd auto && ../comp-so fs_split.cc

auto/html_br.cc: ac_str conf/html_br
	head -n 1 conf/html_br | ./ac_str vqwww html_br a

auto/html_br.o: comp-so cconf.h auto/html_br.cc
	cd auto && ../comp-so html_br.cc

auto/iconv_in_type.def: try/iconv1.c compile iconv.inc
	(cd try && ../compile iconv1.c `cat ../iconv.inc` > /dev/null 2>&1 \
	&& echo -DICONV_IN_TYPE=const char \
	|| echo -DICONV_IN_TYPE=char ) > auto/iconv_in_type.def
	rm -f try/iconv1.o

auto/locales.cc: ac_gen
	./ac_gen vqwww cmapconf locales 

auto/locales.o: comp-so auto/locales.cc cconf.h
	cd auto && ../comp-so locales.cc

auto/pass_maxl.cc: ac_int conf/pass_maxl
	head -n 1 conf/pass_maxl | ./ac_int vqwww pass_maxl

auto/pass_maxl.o: comp-so auto/pass_maxl.cc cconf.h
	cd auto && ../comp-so pass_maxl.cc

auto/pass_minl.cc: ac_int conf/pass_minl
	head -n 1 conf/pass_minl | ./ac_int vqwww pass_minl

auto/pass_minl.o: comp-so auto/pass_minl.cc cconf.h
	cd auto && ../comp-so pass_minl.cc

auto/pass_ok.cc: ac_str conf/pass_ok
	head -n 1 conf/pass_ok | ./ac_str vqwww pass_ok a

auto/pass_ok.o: comp-so auto/pass_ok.cc cconf.h
	cd auto && ../comp-so pass_ok.cc

auto/redir_max.cc: ac_int conf/redir_max 
	head -n 1 conf/redir_max | ./ac_int vqwww redir_max

auto/redir_max.o: comp-so auto/redir_max.cc cconf.h
	cd auto && ../comp-so redir_max.cc

auto/sess_fmode.cc: ac_int conf/sess_fmode
	head -n 1 conf/sess_fmode | ./ac_int vqwww sess_fmode

auto/sess_fmode.o: comp-so auto/sess_fmode.cc cconf.h
	cd auto && ../comp-so sess_fmode.cc

auto/sess_timeout.cc: ac_int conf/sess_timeout
	head -n 1 conf/sess_timeout | ./ac_int vqwww sess_timeout 60

auto/sess_timeout.o: comp-so auto/sess_timeout.cc cconf.h
	cd auto && ../comp-so sess_timeout.cc

auto/user_maxl.cc: ac_int conf/user_maxl
	head -n 1 conf/user_maxl | ./ac_int vqwww user_maxl

auto/user_maxl.o: comp-so auto/user_maxl.cc cconf.h
	cd auto && ../comp-so user_maxl.cc

auto/user_minl.cc: ac_int conf/user_minl
	head -n 1 conf/user_minl | ./ac_int vqwww user_minl

auto/user_minl.o: comp-so auto/user_minl.cc cconf.h
	cd auto && ../comp-so user_minl.cc
	
auto/user_ok.cc: ac_str conf/user_ok
	head -n 1 conf/user_ok | ./ac_str vqwww user_ok a

auto/user_ok.o: comp-so auto/user_ok.cc cconf.h
	cd auto && ../comp-so user_ok.cc

auto/vq_gid.cc: getgid ac_int conf/vq_user
	./getgid "`head -n 1 conf/vq_user`" | ./ac_int vqwww vq_gid 1 cgidconf
	
auto/vq_gid.o: comp-so auto/vq_gid.cc cconf.h
	cd auto && ../comp-so vq_gid.cc

auto/vq_uid.cc: getuid ac_int conf/vq_user
	./getuid "`head -n 1 conf/vq_user`" | ./ac_int vqwww vq_uid 1 cuidconf

auto/vq_uid.o: comp-so auto/vq_uid.cc cconf.h
	cd auto && ../comp-so vq_uid.cc

auto/xmlstart.cc: ac_str conf/xmlstart
	head -n 1 conf/xmlstart | ./ac_str vqwww xmlstart a

auto/xmlstart.o: comp-so auto/xmlstart.cc cconf.h
	cd auto && ../comp-so xmlstart.cc

cconfmod.o: compile cconfmod.h cconfmod.cc cmod.h vqwww.h cgicc.inc vq.inc \
cenvironment.h
	./compile cconfmod.cc `cat cgicc.inc vq.inc`

cfilesess.o: comp-so cfilesess.h cfilesess.cc csess.h auto/lmd5.h \
vq.inc vqwww_conf.h cconf.h
	./comp-so cfilesess.cc `cat vq.inc`

libsess.so: libsess.so.4.0
	ln -sf libsess.so.4.0 libsess.so

libsess.so.4.0: make-so cfilesess.o auto/lmd5.o libvqwww_conf.a
	./make-so libsess.so.4.0 cfilesess.o auto/lmd5.o libvqwww_conf.a

cautorespmod.o: compile cmod.h cautorespmod.h cautorespmod.cc vqwww.h \
cgicc.inc vq.inc cgi.h html.h vqwww_conf.h auto/iconv_in_type.def iconv.inc \
cenvironment.h
	./compile cautorespmod.cc `cat cgicc.inc vq.inc iconv.inc` \
	"`cat auto/iconv_in_type.def`"

cgi.o: compile cgi.cc cgicc.inc
	./compile cgi.cc `cat cgicc.inc`

cloghistmod.o: compile cloghistmod.cc cloghistmod.h cmod.h vqwww.h \
cgicc.inc vq.inc cgi.h html.h vqwww_conf.h cenvironment.h
	./compile cloghistmod.cc `cat vq.inc cgicc.inc`

conf_home.cc: str2cc conf/home
	./str2cc vqwww "`head -n 1 conf/home`" home a > conf_home.cc

conf_home.o: compile conf_home.cc conf_home.h
	./compile conf_home.cc

cconf.cc: ../../cconf.cc
	cp -f ../../cconf.cc .

cconf.h: ../../cconf.h
	cp -f ../../cconf.h .

cconf.o: comp-so cconf.cc cconf.h
	./comp-so cconf.cc

cenvironment.o: compile cenvironment.h cenvironment.cc csess.h cgicc.inc \
vq.inc conf_home.h cmod.h sabmain.h
	./compile cenvironment.cc `cat cgicc.inc vq.inc`

coutmod.o: compile cmod.h coutmod.h coutmod.cc vqwww.h cgicc.inc vq.inc \
cgi.h cenvironment.h
	./compile coutmod.cc `cat cgicc.inc vq.inc`

cpassmod.o: compile cmod.h cpassmod.h cpassmod.cc vqwww.h cgicc.inc vq.inc \
cgi.h html.h vqwww_conf.h cconf.h cenvironment.h
	./compile cpassmod.cc `cat cgicc.inc vq.inc`

credirmod.o: compile cmod.h credirmod.h credirmod.cc vqwww.h cgicc.inc \
vq.inc cgi.h cenvironment.h
	./compile credirmod.cc `cat cgicc.inc vq.inc`

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

hier.o: compile hier.cc install.h conf_home.h vqwww_conf.h cconf.h
	./compile hier.cc

html.o: compile html.cc html.h
	./compile html.cc

install.cc: ../../install.cc
	cp -f ../../install.cc .

install.h: ../../install.h
	cp -f ../../install.h .

install: load install.o hier.o conf_home.o libvqwww_conf.a
	./load install hier.o conf_home.o libvqwww_conf.a

install.o: compile install.cc install.h
	./compile install.cc

int2cc: load int2cc.o
	./load int2cc

int2cc.o: compile int2cc.cc
	./compile int2cc.cc

auto/lmd5.c: compile try/md5.c md5.c1 md5.c2
	( ( cd try && ../compile md5.c ) >/dev/null 2>&1 \
	&& cat md5.c1 || cat md5.c2 ) > auto/lmd5.c
	rm -f try/md5.o 

auto/lmd5.h: compile try/md5.c md5.h1 md5.h2
	( ( cd try && ./compile md5.c ) >/dev/null 2>&1 \
	&& cat md5.h1 || cat md5.h2 ) > auto/lmd5.h
	rm -f try/md5.o

auto/lmd5.o: comp-so auto/lmd5.c auto/lmd5.h
	cd auto && ../comp-so lmd5.c

sabmain.o: compile sab.inc sabmain.cc conf_home.h vqwww_conf.h cconf.h
	./compile sabmain.cc `cat sab.inc`

str2cc: load str2cc.o
	./load str2cc

str2cc.cc: ../../str2cc.cc
	cp -f ../../str2cc.cc .
	
str2cc.o: compile str2cc.cc
	./compile str2cc.cc
	
vqwww: load vqwww.o sabmain.o cconfmod.o cpassmod.o sab.lib cgicc.lib \
cgi.o html.o coutmod.o credirmod.o vq.lib cgi.o \
conf_home.o libsess.so auto/dl.lib iconv.lib \
cautorespmod.o cloghistmod.o libvqwww_conf.a cenvironment.o
	./load vqwww sabmain.o cconfmod.o cpassmod.o \
	html.o coutmod.o credirmod.o cautorespmod.o cloghistmod.o cenvironment.o \
	cgi.o conf_home.o libvqwww_conf.a \
	`cat vq.lib` `cat sab.lib cgicc.lib` -lsess `cat auto/dl.lib` \
	`cat iconv.lib`

vqwww.o: compile cmod.h cconfmod.h cpassmod.h vqwww.cc cgicc.inc sabmain.h \
csess.h cgi.h vqwww_conf.h cconf.h cautorespmod.h \
vq.inc coutmod.h conf_home.h cloghistmod.h cenvironment.h cmod.h
	./compile vqwww.cc `cat cgicc.inc vq.inc`

libvqwww_conf.a: make-lib auto/fs.o auto/fs_split.o auto/html_br.o \
auto/pass_maxl.o auto/pass_minl.o auto/pass_ok.o auto/redir_max.o \
auto/sess_timeout.o auto/user_maxl.o auto/user_minl.o auto/user_ok.o \
auto/vq_gid.o auto/vq_uid.o auto/xmlstart.o auto/sess_fmode.o \
auto/codepages.o auto/locales.o cconf.o
	./make-lib libvqwww_conf.a auto/fs.o auto/fs_split.o auto/html_br.o \
	auto/pass_maxl.o auto/pass_minl.o auto/pass_ok.o auto/redir_max.o \
	auto/sess_timeout.o auto/user_maxl.o auto/user_minl.o auto/user_ok.o \
	auto/vq_gid.o auto/vq_uid.o auto/xmlstart.o auto/sess_fmode.o \
	auto/codepages.o auto/locales.o cconf.o
	
#####################
clean:
		rm -rf `cat TARGETS`

