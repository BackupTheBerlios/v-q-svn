.PHONY: auto-mkdir

it: auto-mkdir libhosting.so wwwalichg install stat_auth

ac_int: load ac_int.o conf_home.o
	./load ac_int conf_home.o

ac_int.cc: ../../ac_int.cc
	cp -f ../../ac_int.cc .

ac_int.o: compile ac_int.cc conf_home.h
	./compile ac_int.cc -DHOME_NS=hosting

ac_str: load ac_str.o conf_home.o
	./load ac_str conf_home.o

ac_str.cc: ../../ac_str.cc
	cp -f ../../ac_str.cc .

ac_str.o: compile ac_str.cc conf_home.h
	./compile ac_str.cc -DHOME_NS=hosting

auto-mkdir:
	mkdir -p auto

cconf.cc: ../../cconf.cc
	cp -f ../../cconf.cc .

cconf.h: ../../cconf.h
	cp -f ../../cconf.h .

cconf.o: comp-so cconf.h cconf.cc
	./comp-so cconf.cc

cpginfodb.o: comp-so cpginfodb.cc cpginfodb.h cinfodb.h pgsql.inc \
cconf.h hosting_conf.h vq.inc
	./comp-so cpginfodb.cc `cat pgsql.inc vq.inc`

auto/ftp_base.cc: ac_str conf/ftp_base
	head -n 1 conf/ftp_base | ./ac_str hosting ftp_base a

auto/ftp_base.o: comp-so cconf.h auto/ftp_base.cc
	cd auto && ../comp-so ftp_base.cc

auto/ftp_ok.cc: ac_str conf/ftp_ok
	head -n 1 conf/ftp_ok | ./ac_str hosting ftp_ok a

auto/ftp_ok.o: comp-so cconf.h auto/ftp_ok.cc
	cd auto && ../comp-so ftp_ok.cc

auto/html_br.cc: ac_str conf/html_br
	head -n 1 conf/html_br | ./ac_str hosting html_br a

auto/html_br.o: comp-so cconf.h auto/html_br.cc
	cd auto && ../comp-so html_br.cc

auto/vq_gid.cc: getgid ac_int conf/vq_user
	./getgid "`head -n 1 conf/vq_user`" | ./ac_int hosting vq_gid 1 cgidconf
	
auto/vq_gid.o: comp-so auto/vq_gid.cc cconf.h
	cd auto && ../comp-so vq_gid.cc

auto/vq_uid.cc: getuid ac_int conf/vq_user
	./getuid "`head -n 1 conf/vq_user`" | ./ac_int hosting vq_uid 1 cuidconf

auto/vq_uid.o: comp-so auto/vq_uid.cc cconf.h
	cd auto && ../comp-so vq_uid.cc

conf_home.cc: str2cc conf/home
	./str2cc hosting "`head -n 1 conf/home`" home a > conf_home.cc

conf_home.o: comp-so conf_home.cc conf_home.h
	./comp-so conf_home.cc

auto/notify.cc: ac_str conf/notify
	head -n 1 conf/notify | ./ac_str hosting notify a

auto/notify.o: comp-so cconf.h auto/notify.cc
	cd auto && ../comp-so notify.cc

auto/pass_maxl.cc: ac_int conf/pass_maxl
	head -n 1 conf/pass_maxl | ./ac_int hosting pass_maxl

auto/pass_maxl.o: comp-so auto/pass_maxl.cc cconf.h
	cd auto && ../comp-so pass_maxl.cc

auto/pass_minl.cc: ac_int conf/pass_minl
	head -n 1 conf/pass_minl | ./ac_int hosting pass_minl

auto/pass_minl.o: comp-so auto/pass_minl.cc cconf.h
	cd auto && ../comp-so pass_minl.cc

auto/pass_ok.cc: ac_str conf/pass_ok
	head -n 1 conf/pass_ok | ./ac_str hosting pass_ok a

auto/pass_ok.o: comp-so auto/pass_ok.cc cconf.h
	cd auto && ../comp-so pass_ok.cc

auto/user_maxl.cc: ac_int conf/user_maxl
	head -n 1 conf/user_maxl | ./ac_int hosting user_maxl

auto/user_maxl.o: comp-so auto/user_maxl.cc cconf.h
	cd auto && ../comp-so user_maxl.cc

auto/user_minl.cc: ac_int conf/user_minl
	head -n 1 conf/user_minl | ./ac_int hosting user_minl

auto/user_minl.o: comp-so auto/user_minl.cc cconf.h
	cd auto && ../comp-so user_minl.cc
	
auto/user_ok.cc: ac_str conf/user_ok
	head -n 1 conf/user_ok | ./ac_str hosting user_ok a

auto/user_ok.o: comp-so auto/user_ok.cc cconf.h
	cd auto && ../comp-so user_ok.cc

cftpmod.o: comp-so cftpmod.h cftpmod.cc cinfodb.h vqwww.inc vq.inc \
cconf.h hosting_conf.h cgicc.inc
	./comp-so cftpmod.cc `cat vqwww.inc vq.inc cgicc.inc`

auto/pgsql.cc: ac_str conf/pgsql
	head -n 1 conf/pgsql | ./ac_str hosting pgsql a

auto/pgsql.o: comp-so cconf.h auto/pgsql.cc
	cd auto && ../comp-so pgsql.cc

auto/prefix_ok.o: comp-so cconf.h auto/prefix_ok.cc
	cd auto && ../comp-so prefix_ok.cc

auto/prefix_ok.cc: ac_str conf/prefix_ok
	head -n 1 conf/prefix_ok | ./ac_str hosting prefix_ok a

auto/wwwali_exe.o: comp-so cconf.h auto/wwwali_exe.cc
	cd auto && ../comp-so wwwali_exe.cc

auto/wwwali_exe.cc: ac_str conf/wwwali_exe
	head -n 1 conf/wwwali_exe | ./ac_str hosting wwwali_exe a

cuseraddmod.o: comp-so cuseraddmod.h cuseraddmod.cc cinfodb.h vqwww.inc \
cconf.h hosting_conf.h vq.inc cgicc.inc
	./comp-so cuseraddmod.cc `cat vqwww.inc vq.inc cgicc.inc`

cusersmod.o: comp-so cusersmod.h cusersmod.cc cinfodb.h vqwww.inc vq.inc \
cgicc.inc
	./comp-so cusersmod.cc `cat vqwww.inc vq.inc cgicc.inc`

cwwwalimod.o: comp-so cwwwalimod.cc cwwwalimod.h cinfodb.h vqwww.inc vq.inc \
hosting_conf.h cconf.h cgicc.inc
	./comp-so cwwwalimod.cc `cat vqwww.inc vq.inc cgicc.inc`

cwwwstatmod.o: comp-so cwwwstatmod.cc cwwwstatmod.h cinfodb.h vqwww.inc vq.inc \
cgicc.inc
	./comp-so cwwwstatmod.cc `cat vqwww.inc` `cat vq.inc cgicc.inc`

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

hier.o: compile hier.cc conf_home.h install.h hosting_conf.h cconf.h
	./compile hier.cc
	
hosting.o: comp-so hosting.cc cftpmod.h cwwwalimod.h vqwww.inc \
vq.inc cusersmod.h cuseraddmod.h cwwwstatmod.h cgicc.inc
	./comp-so hosting.cc `cat vqwww.inc vq.inc cgicc.inc`

hosting_conf.a: make-lib auto/ftp_base.o auto/ftp_ok.o \
auto/notify.o auto/pgsql.o auto/prefix_ok.o auto/wwwali_exe.o auto/html_br.o \
auto/vq_gid.o auto/vq_uid.o cconf.o 
	./make-lib hosting_conf.a auto/ftp_base.o auto/ftp_ok.o \
	auto/notify.o auto/pgsql.o auto/prefix_ok.o auto/wwwali_exe.o \
	auto/html_br.o auto/vq_gid.o auto/vq_uid.o cconf.o

install.cc: ../../install.cc
	cp -f ../../install.cc .

install.h: ../../install.h
	cp -f ../../install.h .
	
install: load install.o conf_home.o hier.o hosting_conf.a
	./load install hier.o hosting_conf.a conf_home.o

install.o: compile install.cc install.h
	./compile install.cc

libhosting.so: make-so cftpmod.o cpginfodb.o pgsql.lib \
pgsql.lib cwwwalimod.o hosting.o \
cusersmod.o vq.lib cuseraddmod.o \
cwwwstatmod.o hosting_conf.a
	./make-so hosting cftpmod.o cpginfodb.o \
	cwwwalimod.o hosting.o \
	cusersmod.o cuseraddmod.o cwwwstatmod.o hosting_conf.a \
	-lvqwww_conf `cat pgsql.lib vq.lib` -L/usr/local/vqwww/lib/vqwww

stat_auth: load stat_auth.o cgicc.lib vqwww.lib vq.lib
	./load stat_auth `cat cgicc.lib` `cat vqwww.lib` `cat vq.lib` -lsess

stat_auth.o: compile stat_auth.cc vqwww.inc cgicc.inc
	./compile stat_auth.cc `cat vqwww.inc` `cat cgicc.inc`

str2cc: str2cc.o load
	./load str2cc

str2cc.cc: ../../str2cc.cc
	cp -f ../../str2cc.cc .

str2cc.o: comp-so str2cc.cc
	./comp-so str2cc.cc

wwwalichg: load wwwalichg.o
	./load wwwalichg

wwwalichg.o: compile wwwalichg.cc
	./compile wwwalichg.cc

clean:
	rm -rf `cat TARGETS`
