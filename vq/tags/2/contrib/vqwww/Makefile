all: vqwww install

#######################
cconfmod.o: compile cconfmod.h cconfmod.cc cmod.h vqwww.h cgicc.inc vq.inc
	./compile cconfmod.cc `cat cgicc.inc vq.inc`

cfilesess.o: compile cfilesess.h cfilesess.cc conf_fs_split.h csess.h lmd5.h \
vq.inc conf_fs.h
	./compile cfilesess.cc `cat vq.inc`

dl.lib: trydl.c compile
	( ./compile trydl.c -ldl >/dev/null 2>&1 \
	&& echo "" || echo -ldl ) > dl.lib
	rm -f trydl.o 

libsess.so: libsess.so.1
	ln -sf libsess.so.1 libsess.so

libsess.so.1: make-so cfilesess.o conf_fs.o conf_fs_split.o lmd5.o
	./make-so libsess.so.1 cfilesess.o conf_fs.o conf_fs_split.o lmd5.o

cgi.o: compile cgi.cc cgicc.inc
	./compile cgi.cc `cat cgicc.inc`

conf_fs.cc: str2cc conf/fs
	./str2cc "`head -n 1 conf/fs`" fs a > conf_fs.cc

conf_fs.o: comp-so conf_fs.cc
	./comp-so conf_fs.cc

conf_fs_split.cc: int2cc conf/fs_split
	./int2cc "`head -n 1 conf/fs_split`" fs_split > conf_fs_split.cc

conf_fs_split.o: comp-so conf_fs_split.cc
	./comp-so conf_fs_split.cc

conf_home.cc: str2cc conf/home
	./str2cc "`head -n 1 conf/home`" home a > conf_home.cc

conf_home.o: compile conf_home.cc conf_home.h
	./compile conf_home.cc

conf_html_br.cc: str2cc conf/html_br
	./str2cc "`head -n 1 conf/html_br`" html_br a > conf_html_br.cc

conf_html_br.o: compile conf_html_br.h conf_html_br.cc
	./compile conf_html_br.cc

conf_pass_maxl.cc: int2cc conf/pass_maxl
	./int2cc "`head -n 1 conf/pass_maxl`" pass_maxl > conf_pass_maxl.cc

conf_pass_maxl.o: compile conf_pass_maxl.cc conf_pass_maxl.h
	./compile conf_pass_maxl.cc

conf_pass_minl.cc: int2cc conf/pass_minl
	./int2cc "`head -n 1 conf/pass_minl`" pass_minl > conf_pass_minl.cc

conf_pass_minl.o: compile conf_pass_minl.cc conf_pass_minl.h
	./compile conf_pass_minl.cc

conf_pass_ok.cc: str2cc conf/pass_ok
	./str2cc "`head -n 1 conf/pass_ok`" pass_ok a > conf_pass_ok.cc

conf_pass_ok.o: compile conf_pass_ok.cc conf_pass_ok.h
	./compile conf_pass_ok.cc

conf_redir_max.cc: int2cc conf/redir_max
	./int2cc "`head -n 1 conf/redir_max`" redir_max > conf_redir_max.cc

conf_redir_max.o: compile conf_redir_max.h conf_redir_max.cc
	./compile conf_redir_max.cc

conf_sess_timeout.cc: int2cc conf/sess_timeout
	./int2cc "`head -n 1 conf/sess_timeout`" sess_timeout 60 > conf_sess_timeout.cc

conf_sess_timeout.o: compile conf_sess_timeout.cc
	./compile conf_sess_timeout.cc

conf_user_maxl.cc: int2cc conf/user_maxl
	./int2cc "`head -n 1 conf/user_maxl`" user_maxl > conf_user_maxl.cc

conf_user_maxl.o: compile conf_user_maxl.cc conf_user_maxl.h
	./compile conf_user_maxl.cc

conf_user_minl.cc: int2cc conf/user_minl
	./int2cc "`head -n 1 conf/user_minl`" user_minl > conf_user_minl.cc

conf_user_minl.o: compile conf_user_minl.cc conf_user_minl.h
	./compile conf_user_minl.cc
	
conf_user_ok.cc: str2cc conf/user_ok
	./str2cc "`head -n 1 conf/user_ok`" user_ok a > conf_user_ok.cc

conf_user_ok.o: compile conf_user_ok.cc conf_user_ok.h
	./compile conf_user_ok.cc

conf_vq_gid.cc: getgid conf/vq_user
	./getgid "`head -n 1 conf/vq_user`" vq_gid > conf_vq_gid.cc.tmp \
	&& mv conf_vq_gid.cc.tmp conf_vq_gid.cc
	
conf_vq_gid.o: compile conf_vq_gid.cc conf_vq_gid.h
	./compile conf_vq_gid.cc

conf_vq_uid.cc: getuid conf/vq_user
	./getuid "`head -n 1 conf/vq_user`" vq_uid > conf_vq_uid.cc.tmp \
	&& mv conf_vq_uid.cc.tmp conf_vq_uid.cc

conf_vq_uid.o: compile conf_vq_uid.cc conf_vq_uid.h
	./compile conf_vq_uid.cc

conf_xmlstart.cc: str2cc conf/xmlstart
	./str2cc "`head -n 1 conf/xmlstart`" xmlstart a > conf_xmlstart.cc

conf_xmlstart.o: compile conf_xmlstart.cc
	./compile conf_xmlstart.cc

conf_xslt_file.cc: str2cc conf/xslt_file
	./str2cc "`head -n 1 conf/xslt_file`" xslt_file a > conf_xslt_file.cc

conf_xslt_file.o: compile conf_xslt_file.cc
	./compile conf_xslt_file.cc

conf_xsltsheet.o: compile conf_xsltsheet.cc conf_xsltsheet.h conf_xslt_file.h \
conf_home.h
	./compile conf_xsltsheet.cc

conf_xsltsheet.a: make-lib conf_xsltsheet.o conf_xslt_file.o conf_home.o
	./make-lib conf_xsltsheet conf_xslt_file.o conf_home.o

coutmod.o: compile cmod.h coutmod.h coutmod.cc vqwww.h cgicc.inc vq.inc \
cgi.h
	./compile coutmod.cc `cat cgicc.inc vq.inc`

cpassmod.a: make-lib cpassmod.o conf_pass_maxl.o conf_pass_minl.o
	./make-lib cpassmod conf_pass_maxl.o conf_pass_minl.o
	
cpassmod.o: compile cmod.h cpassmod.h cpassmod.cc vqwww.h cgicc.inc vq.inc \
cgi.h conf_pass_ok.h html.h conf_html_br.h
	./compile cpassmod.cc `cat cgicc.inc vq.inc`

credirmod.a: make-lib credirmod.o conf_redir_max.o 
	./make-lib credirmod conf_redir_max.o

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

html.o: compile html.cc html.h
	./compile html.cc

install: load install.o conf_home.o conf_vq_uid.o conf_vq_gid.o
	./load install conf_home.o conf_vq_uid.o conf_vq_gid.o

install.o: compile install.cc conf_home.h conf_vq_uid.h conf_vq_gid.h
	./compile install.cc

int2cc: load int2cc.o
	./load int2cc

int2cc.o: compile int2cc.cc
	./compile int2cc.cc

lmd5.c: compile trymd5.c md5.c1 md5.c2
	( ./compile trymd5.c >/dev/null 2>&1 \
	&& cat md5.c1 || cat md5.c2 ) > lmd5.c
	rm -f trymd5.o 

lmd5.h: compile trymd5.c md5.h1 md5.h2
	( ./compile trymd5.c >/dev/null 2>&1 \
	&& cat md5.h1 || cat md5.h2 ) > lmd5.h
	rm -f trymd5.o

lmd5.o: comp-so lmd5.c lmd5.h
	./comp-so lmd5.c

sabmain.a: make-lib sabmain.o conf_xmlstart.o conf_xsltsheet.o conf_xslt_file.o 
	./make-lib sabmain conf_xmlstart.o conf_xsltsheet.o conf_xslt_file.o 
	
sabmain.o: compile sab.inc sabmain.cc conf_xmlstart.h conf_xsltsheet.h
	./compile sabmain.cc `cat sab.inc`

str2cc: load str2cc.o
	./load str2cc
	
str2cc.o: compile str2cc.cc
	./compile str2cc.cc
	
vqwww: load vqwww.o sabmain.a cconfmod.o cpassmod.a sab.lib cgicc.lib \
cgi.o conf_sess_timeout.o html.o conf_html_br.o \
conf_user_ok.o conf_pass_ok.o coutmod.o credirmod.a vq.lib cgi.o \
conf_user_maxl.o conf_user_minl.o conf_home.o libsess.so dl.lib
	./load vqwww sabmain.a cconfmod.o cpassmod.a \
	html.o conf_html_br.o conf_user_ok.o conf_pass_ok.o \
	coutmod.o credirmod.a cgi.o conf_sess_timeout.o \
	conf_user_maxl.o conf_user_minl.o conf_home.o \
	`cat vq.lib` `cat sab.lib cgicc.lib` -lsess `cat dl.lib`

vqwww.o: compile cmod.h cconfmod.h cpassmod.h vqwww.cc cgicc.inc sabmain.h \
csess.h cgi.h conf_sess_timeout.h \
vq.inc coutmod.h conf_home.h
	./compile vqwww.cc `cat cgicc.inc vq.inc`

#####################
clean:
		rm -rf `cat TARGETS`
