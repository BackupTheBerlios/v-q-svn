all: auto vqwww install

#######################
auto:
	mkdir auto

ac_gen: load ac_gen.o conf_home.o
	./load ac_gen conf_home.o

ac_gen.o: compile ac_gen.cc conf_home.h
	./compile ac_gen.cc

ac_int: load ac_int.o conf_home.o
	./load ac_int conf_home.o

ac_int.o: compile ac_int.cc conf_home.h
	./compile ac_int.cc

ac_str: load ac_str.o conf_home.o
	./load ac_str conf_home.o

ac_str.o: compile ac_str.cc conf_home.h
	./compile ac_str.cc

auto/dl.lib: try/dl.c compile load
	( cd try && ../compile dl.c && ../load dl -ldl >/dev/null 2>&1 \
	&& echo -ldl || echo "" ) > auto/dl.lib
	rm -f try/dl.o try/dl

auto/codepages.cc: ac_gen
	./ac_gen cmapconf codepages 

auto/codepages.o: comp-so auto/codepages.cc cconf.h
	cd auto && ../comp-so codepages.cc

auto/fs.cc: ac_str conf/fs
	head -n 1 conf/fs | ./ac_str fs a

auto/fs.o: comp-so auto/fs.cc cconf.h
	cd auto && ../comp-so fs.cc

auto/fs_split.cc: ac_int conf/fs_split
	head -n 1 conf/fs_split | ./ac_int fs_split

auto/fs_split.o: comp-so auto/fs_split.cc cconf.h
	cd auto && ../comp-so fs_split.cc

auto/html_br.cc: ac_str conf/html_br
	head -n 1 conf/html_br | ./ac_str html_br a

auto/html_br.o: comp-so cconf.h auto/html_br.cc
	cd auto && ../comp-so html_br.cc

auto/iconv.lib: try/iconv.c compile load
	( cd try && ../compile iconv.c -I /usr/local/include \
	&& ../load iconv >/dev/null 2>&1 \
	&& echo "" || echo -liconv ) > auto/iconv.lib
	rm -f try/iconv.o try/iconv

auto/iconv_in_type.def: try/iconv1.c compile
	(cd try && ../compile iconv1.c -I/usr/local/include > /dev/null 2>&1 \
	&& echo -DICONV_IN_TYPE=const char \
	|| echo -DICONV_IN_TYPE=char ) > auto/iconv_in_type.def
	rm -f try/iconv1.o

auto/pass_maxl.cc: ac_int conf/pass_maxl
	head -n 1 conf/pass_maxl | ./ac_int pass_maxl

auto/pass_maxl.o: comp-so auto/pass_maxl.cc cconf.h
	cd auto && ../comp-so pass_maxl.cc

auto/pass_minl.cc: ac_int conf/pass_minl
	head -n 1 conf/pass_minl | ./ac_int pass_minl

auto/pass_minl.o: comp-so auto/pass_minl.cc cconf.h
	cd auto && ../comp-so pass_minl.cc

auto/pass_ok.cc: ac_str conf/pass_ok
	head -n 1 conf/pass_ok | ./ac_str pass_ok a

auto/pass_ok.o: comp-so auto/pass_ok.cc cconf.h
	cd auto && ../comp-so pass_ok.cc

auto/redir_max.cc: ac_int conf/redir_max 
	head -n 1 conf/redir_max | ./ac_int redir_max

auto/redir_max.o: comp-so auto/redir_max.cc cconf.h
	cd auto && ../comp-so redir_max.cc

auto/sess_fmode.cc: ac_int conf/sess_fmode
	head -n 1 conf/sess_fmode | ./ac_int sess_fmode

auto/sess_fmode.o: comp-so auto/sess_fmode.cc cconf.h
	cd auto && ../comp-so sess_fmode.cc

auto/sess_timeout.cc: ac_int conf/sess_timeout
	head -n 1 conf/sess_timeout | ./ac_int sess_timeout 60

auto/sess_timeout.o: comp-so auto/sess_timeout.cc cconf.h
	cd auto && ../comp-so sess_timeout.cc

auto/user_maxl.cc: ac_int conf/user_maxl
	head -n 1 conf/user_maxl | ./ac_int user_maxl

auto/user_maxl.o: comp-so auto/user_maxl.cc cconf.h
	cd auto && ../comp-so user_maxl.cc

auto/user_minl.cc: ac_int conf/user_minl
	head -n 1 conf/user_minl | ./ac_int user_minl

auto/user_minl.o: comp-so auto/user_minl.cc cconf.h
	cd auto && ../comp-so user_minl.cc
	
auto/user_ok.cc: ac_str conf/user_ok
	head -n 1 conf/user_ok | ./ac_str user_ok a

auto/user_ok.o: comp-so auto/user_ok.cc cconf.h
	cd auto && ../comp-so user_ok.cc

auto/vq_gid.cc: getgid ac_int conf/vq_user
	./getgid "`head -n 1 conf/vq_user`" | ./ac_int vq_gid 1 cgidconf
	
auto/vq_gid.o: comp-so auto/vq_gid.cc cconf.h
	cd auto && ../comp-so vq_gid.cc

auto/vq_uid.cc: getuid ac_int conf/vq_user
	./getuid "`head -n 1 conf/vq_user`" | ./ac_int vq_uid 1 cuidconf

auto/vq_uid.o: comp-so auto/vq_uid.cc cconf.h
	cd auto && ../comp-so vq_uid.cc

auto/xmlstart.cc: ac_str conf/xmlstart
	head -n 1 conf/xmlstart | ./ac_str xmlstart a

auto/xmlstart.o: comp-so auto/xmlstart.cc cconf.h
	cd auto && ../comp-so xmlstart.cc

cconfmod.o: compile cconfmod.h cconfmod.cc cmod.h vqwww.h cgicc.inc vq.inc
	./compile cconfmod.cc `cat cgicc.inc vq.inc`

cfilesess.o: comp-so cfilesess.h cfilesess.cc csess.h auto/lmd5.h \
vq.inc vqwww_conf.h cconf.h
	./comp-so cfilesess.cc `cat vq.inc`

libsess.so: libsess.so.1
	ln -sf libsess.so.1 libsess.so

libsess.so.1: make-so cfilesess.o auto/lmd5.o vqwww_conf.a
	./make-so libsess.so.1 cfilesess.o auto/lmd5.o vqwww_conf.a

cautorespmod.o: compile cmod.h cautorespmod.h cautorespmod.cc vqwww.h \
cgicc.inc vq.inc cgi.h html.h vqwww_conf.h auto/iconv_in_type.def
	./compile cautorespmod.cc `cat cgicc.inc vq.inc` \
	"`cat auto/iconv_in_type.def`"

cgi.o: compile cgi.cc cgicc.inc
	./compile cgi.cc `cat cgicc.inc`

conf_home.cc: str2cc conf/home
	./str2cc "`head -n 1 conf/home`" home a > conf_home.cc

conf_home.o: compile conf_home.cc conf_home.h
	./compile conf_home.cc

cconf.o: comp-so cconf.cc cconf.h
	./comp-so cconf.cc

coutmod.o: compile cmod.h coutmod.h coutmod.cc vqwww.h cgicc.inc vq.inc \
cgi.h
	./compile coutmod.cc `cat cgicc.inc vq.inc`

cpassmod.o: compile cmod.h cpassmod.h cpassmod.cc vqwww.h cgicc.inc vq.inc \
cgi.h html.h vqwww_conf.h cconf.h
	./compile cpassmod.cc `cat cgicc.inc vq.inc`

credirmod.o: compile cmod.h credirmod.h credirmod.cc vqwww.h cgicc.inc \
vq.inc cgi.h
	./compile credirmod.cc `cat cgicc.inc vq.inc`

getgid: load getgid.o
	./load getgid

getgid.o: compile getgid.cc
	./compile getgid.cc

getuid: load getuid.o
	./load getuid

getuid.o: compile getuid.cc
	./compile getuid.cc

hier.o: compile hier.cc install.h conf_home.h vqwww_conf.h cconf.h
	./compile hier.cc

html.o: compile html.cc html.h
	./compile html.cc

install: load install.o hier.o conf_home.o vqwww_conf.a
	./load install hier.o conf_home.o vqwww_conf.a

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
	
str2cc.o: compile str2cc.cc
	./compile str2cc.cc
	
vqwww: load vqwww.o sabmain.o cconfmod.o cpassmod.o sab.lib cgicc.lib \
cgi.o auto/sess_timeout.o html.o coutmod.o credirmod.o vq.lib cgi.o \
conf_home.o libsess.so auto/dl.lib auto/iconv.lib \
cautorespmod.o vqwww_conf.a
	./load vqwww sabmain.o cconfmod.o cpassmod.o \
	html.o coutmod.o credirmod.o cautorespmod.o cgi.o \
	conf_home.o auto/fs.o auto/fs_split.o auto/html_br.o \
	auto/pass_maxl.o auto/pass_minl.o auto/pass_ok.o auto/redir_max.o \
	auto/sess_timeout.o auto/user_maxl.o auto/user_minl.o auto/user_ok.o \
	auto/vq_gid.o auto/vq_uid.o auto/xmlstart.o auto/sess_fmode.o \
	auto/codepages.o cconf.o \
	`cat vq.lib` `cat sab.lib cgicc.lib` -lsess `cat auto/dl.lib` \
	`cat auto/iconv.lib`

vqwww.o: compile cmod.h cconfmod.h cpassmod.h vqwww.cc cgicc.inc sabmain.h \
csess.h cgi.h vqwww_conf.h cconf.h cautorespmod.h \
vq.inc coutmod.h conf_home.h
	./compile vqwww.cc `cat cgicc.inc vq.inc`

vqwww_conf.a: make-lib auto/fs.o auto/fs_split.o auto/html_br.o \
auto/pass_maxl.o auto/pass_minl.o auto/pass_ok.o auto/redir_max.o \
auto/sess_timeout.o auto/user_maxl.o auto/user_minl.o auto/user_ok.o \
auto/vq_gid.o auto/vq_uid.o auto/xmlstart.o auto/sess_fmode.o \
auto/codepages.o cconf.o
	./make-lib vqwww_conf.a auto/fs.o auto/fs_split.o auto/html_br.o \
	auto/pass_maxl.o auto/pass_minl.o auto/pass_ok.o auto/redir_max.o \
	auto/sess_timeout.o auto/user_maxl.o auto/user_minl.o auto/user_ok.o \
	auto/vq_gid.o auto/vq_uid.o auto/xmlstart.o auto/sess_fmode.o \
	auto/codepages.o cconf.o
	
#####################
clean:
		rm -rf `cat TARGETS`

