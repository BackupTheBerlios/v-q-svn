it: auto libhosting.so wwwalichg install stat_auth

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

conf_home.cc: str2cc conf/home
	./str2cc hosting "`head -n 1 conf/home`" home a > conf_home.cc

conf_home.o: comp-so conf_home.cc conf_home.h
	./comp-so conf_home.cc

auto/notify.cc: ac_str conf/notify
	head -n 1 conf/notify | ./ac_str hosting notify a

auto/notify.o: comp-so cconf.h auto/notify.cc
	cd auto && ../comp-so notify.cc

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

hier.o: compile hier.cc conf_home.h install.h
	./compile hier.cc
	
hosting.o: comp-so hosting.cc cftpmod.h cwwwalimod.h vqwww.inc \
vq.inc cusersmod.h cuseraddmod.h cwwwstatmod.h cgicc.inc
	./comp-so hosting.cc `cat vqwww.inc vq.inc cgicc.inc`

hosting_conf.a: make-lib auto/ftp_base.o auto/ftp_ok.o \
auto/notify.o auto/pgsql.o auto/prefix_ok.o auto/wwwali_exe.o cconf.o
	./make-lib hosting_conf.a auto/ftp_base.o auto/ftp_ok.o \
	auto/notify.o auto/pgsql.o auto/prefix_ok.o auto/wwwali_exe.o \
	cconf.o

infodb_init.o: comp-so infodb_init.cc cinfodb.h cpginfodb.h pgsql.inc
	./comp-so infodb_init.cc `cat pgsql.inc`

install: load install.o conf_home.o hier.o
	./load install hier.o conf_home.o

install.o: compile install.cc 
	./compile install.cc

libhosting.so: make-so cftpmod.o cpginfodb.o infodb_init.o pgsql.lib \
pgsql.lib cwwwalimod.o hosting.o \
cusersmod.o vq.lib cuseraddmod.o \
cwwwstatmod.o hosting_conf.a
	./make-so hosting cftpmod.o cpginfodb.o infodb_init.o \
	cwwwalimod.o hosting.o \
	cusersmod.o cuseraddmod.o cwwwstatmod.o hosting_conf.a \
	`cat pgsql.lib vq.lib`

stat_auth: load stat_auth.o cgicc.lib vqwww.lib vq.lib
	./load stat_auth `cat cgicc.lib` `cat vqwww.lib` `cat vq.lib` -lsess

stat_auth.o: compile stat_auth.cc vqwww.inc cgicc.inc
	./compile stat_auth.cc `cat vqwww.inc` `cat cgicc.inc`

str2cc: str2cc.o load
	./load str2cc

str2cc.o: comp-so str2cc.cc
	./comp-so str2cc.cc

wwwalichg: load wwwalichg.o
	./load wwwalichg

wwwalichg.o: compile wwwalichg.cc
	./compile wwwalichg.cc

clean:
	rm -rf `cat TARGETS`
