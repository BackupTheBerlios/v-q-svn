it: libhosting.so wwwalichg install stat_auth

cpginfodb.o: comp-so cpginfodb.cc cpginfodb.h cinfodb.h pgsql.inc \
conf_pgsql.h conf_ftp_base.h conf_wwwali_exe.h \
vq.inc
	./comp-so cpginfodb.cc `cat pgsql.inc vq.inc`

conf_ftp_base.cc: str2cc conf/ftp_base
	./str2cc "`head -n 1 conf/ftp_base`" ftp_base a > conf_ftp_base.cc

conf_ftp_base.o: comp-so conf_ftp_base.h conf_ftp_base.cc
	./comp-so conf_ftp_base.cc

conf_ftp_ok.cc: str2cc conf/ftp_ok
	./str2cc "`head -n 1 conf/ftp_ok`" ftp_ok a > conf_ftp_ok.cc

conf_ftp_ok.o: comp-so conf_ftp_ok.h conf_ftp_ok.cc
	./comp-so conf_ftp_ok.cc

conf_home.cc: str2cc conf/home
	./str2cc "`head -n 1 conf/home`" home a > conf_home.cc

conf_home.o: compile conf_home.cc conf_home.h
	./compile conf_home.cc

conf_html_br.cc: str2cc conf/html_br
	./str2cc "`head -n 1 conf/html_br`" html_br a > conf_html_br.cc

conf_html_br.o: comp-so conf_html_br.h conf_html_br.cc
	./comp-so conf_html_br.cc

conf_notify.cc: str2cc conf/notify
	./str2cc "`head -n 1 conf/notify`" notify a > conf_notify.cc

conf_notify.o: compile conf_notify.h conf_notify.cc
	./compile conf_notify.cc

cftpmod.o: comp-so cftpmod.h cftpmod.cc cinfodb.h vqwww.inc vq.inc \
conf_ftp_ok.h
	./comp-so cftpmod.cc `cat vqwww.inc vq.inc`

conf_pgsql.cc: str2cc conf/pgsql
	./str2cc "`head -n 1 conf/pgsql`" pgsql a > conf_pgsql.cc

conf_pgsql.o: comp-so conf_pgsql.h conf_pgsql.cc
	./comp-so conf_pgsql.cc

conf_prefix_ok.o: comp-so conf_prefix_ok.h conf_prefix_ok.cc
	./comp-so conf_prefix_ok.cc

conf_prefix_ok.cc: str2cc conf/prefix_ok
	./str2cc "`head -n 1 conf/prefix_ok`" prefix_ok a > conf_prefix_ok.cc

conf_wwwali_exe.o: comp-so conf_wwwali_exe.h conf_wwwali_exe.cc
	./comp-so conf_wwwali_exe.cc

conf_wwwali_exe.cc: str2cc conf/wwwali_exe
	./str2cc "`head -n 1 conf/wwwali_exe`" wwwali_exe a > conf_wwwali_exe.cc

cuseraddmod.o: comp-so cuseraddmod.h cuseraddmod.cc cinfodb.h vqwww.inc \
conf_notify.h vq.inc 
	./comp-so cuseraddmod.cc `cat vqwww.inc vq.inc`

cusersmod.o: comp-so cusersmod.h cusersmod.cc cinfodb.h vqwww.inc vq.inc
	./comp-so cusersmod.cc `cat vqwww.inc vq.inc`

cwwwalimod.o: comp-so cwwwalimod.cc cwwwalimod.h cinfodb.h vqwww.inc vq.inc \
conf_ftp_ok.h conf_prefix_ok.h
	./comp-so cwwwalimod.cc `cat vqwww.inc vq.inc`

cwwwstatmod.o: comp-so cwwwstatmod.cc cwwwstatmod.h cinfodb.h vqwww.inc vq.inc
	./comp-so cwwwstatmod.cc `cat vqwww.inc` `cat vq.inc`

hosting.o: comp-so hosting.cc cftpmod.h cwwwalimod.h vqwww.inc \
vq.inc cusersmod.h cuseraddmod.h cwwwstatmod.h
	./comp-so hosting.cc `cat vqwww.inc vq.inc`

infodb_init.o: comp-so infodb_init.cc cinfodb.h cpginfodb.h pgsql.inc
	./comp-so infodb_init.cc `cat pgsql.inc`

install: load install.o conf_home.o
	./load install conf_home.o

install.o: compile install.cc conf_home.h
	./compile install.cc

libhosting.so: make-so cftpmod.o cpginfodb.o infodb_init.o pgsql.lib \
conf_pgsql.o conf_ftp_base.o conf_html_br.o \
conf_ftp_ok.o pgsql.lib cwwwalimod.o hosting.o \
conf_prefix_ok.o conf_wwwali_exe.o cusersmod.o vq.lib cuseraddmod.o \
conf_notify.o cwwwstatmod.o
	./make-so hosting cftpmod.o cpginfodb.o infodb_init.o \
	conf_pgsql.o conf_ftp_base.o conf_html_br.o \
	cwwwalimod.o conf_prefix_ok.o conf_ftp_ok.o hosting.o \
	conf_wwwali_exe.o cusersmod.o cuseraddmod.o conf_notify.o cwwwstatmod.o \
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
