all: vqsignup1 install vqsignup1d vqrescue libfreemail.so

setup: all
	./install

#######################
cdbdaemon.o: comp-so cdbdaemon.cc vq.inc conf_home.h
	./comp-so cdbdaemon.cc `cat vq.inc`

cgi.o: compile cgi.cc cgi.h html.h cgicc.inc
	./compile cgi.cc `cat cgicc.inc`

conf_city_minl.cc: int2cc conf/city_minl 
	./int2cc "`head -n 1 conf/city_minl`" city_minl > conf_city_minl.cc

conf_city_minl.o: comp-so conf_city_minl.cc conf_city_minl.h
	./comp-so conf_city_minl.cc

conf_cntry_minl.cc: int2cc conf/cntry_minl 
	./int2cc "`head -n 1 conf/cntry_minl`" cntry_minl > conf_cntry_minl.cc

conf_cntry_minl.o: comp-so conf_cntry_minl.cc conf_cntry_minl.h
	./comp-so conf_cntry_minl.cc

conf_home.h: sstr2cc conf/home
	./sstr2cc "`head -n 1 conf/home`" home a > conf_home.h

conf_html_br.cc: str2cc conf/html_br
	./str2cc "`head -n 1 conf/html_br`" html_br a > conf_html_br.cc

conf_html_br.o: compile conf_html_br.h conf_html_br.cc
	./compile conf_html_br.cc

conf_notify.cc: str2cc conf/notify
	./str2cc "`head -n 1 conf/notify`" notify a > conf_notify.cc

conf_notify.o: compile conf_notify.h conf_notify.cc
	./compile conf_notify.cc

conf_pass_maxl.cc: int2cc conf/pass_maxl
	./int2cc "`head -n 1 conf/pass_maxl`" pass_maxl > conf_pass_maxl.cc

conf_pass_maxl.o: compile conf_pass_maxl.cc conf_pass_maxl.h
	./compile conf_pass_maxl.cc

conf_pass_minl.cc: int2cc conf/pass_minl
	./int2cc "`head -n 1 conf/pass_minl`" pass_minl > conf_pass_minl.cc

conf_pass_minl.o: compile conf_pass_minl.cc conf_pass_minl.h
	./compile conf_pass_minl.cc

conf_pass_ok.cc: str2cc conf/pass_ok
	./str2cc "`head -n 1 conf/pass_ok`" pass_ok > conf_pass_ok.cc

conf_pass_ok.o: compile conf_pass_ok.cc conf_pass_ok.h
	./compile conf_pass_ok.cc

conf_pgsql.cc: str2cc conf/pgsql
	./str2cc "`head -n 1 conf/pgsql`" pgsql a > conf_pgsql.cc

conf_pgsql.o: compile conf_pgsql.h conf_pgsql.cc
	./compile conf_pgsql.cc

conf_user_maxl.cc: int2cc conf/user_maxl
	./int2cc "`head -n 1 conf/user_maxl`" user_maxl > conf_user_maxl.cc

conf_user_maxl.o: compile conf_user_maxl.cc conf_user_maxl.h
	./compile conf_user_maxl.cc

conf_user_minl.cc: int2cc conf/user_minl
	./int2cc "`head -n 1 conf/user_minl`" user_minl > conf_user_minl.cc

conf_user_minl.o: compile conf_user_minl.cc conf_user_minl.h
	./compile conf_user_minl.cc

conf_user_ok.cc: str2cc conf/user_ok
	./str2cc "`head -n 1 conf/user_ok`" user_ok > conf_user_ok.cc
	
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
	./str2cc "`head -n 1 conf/xmlstart`" xmlstart > conf_xmlstart.cc

conf_xmlstart.o: compile conf_xmlstart.cc conf_xmlstart.h
	./compile conf_xmlstart.cc

conf_xmlstart1.cc: str2cc conf/xmlstart1
	./str2cc "`head -n 1 conf/xmlstart1`" xmlstart1 > conf_xmlstart1.cc

conf_xmlstart1.o: compile conf_xmlstart1.cc conf_xmlstart1.h
	./compile conf_xmlstart1.cc

conf_xslt_file.cc: str2cc conf/xslt_file
	./str2cc "`head -n 1 conf/xslt_file`" xslt_file a > conf_xslt_file.cc

conf_xslt_file.o: compile conf_xslt_file.cc conf_xslt_file.h
	./compile conf_xslt_file.cc

conf_xslt_file1.cc: str2cc conf/xslt_file1
	./str2cc "`head -n 1 conf/xslt_file1`" xslt_file1 a > conf_xslt_file1.cc

conf_xslt_file1.o: compile conf_xslt_file1.cc conf_xslt_file1.h
	./compile conf_xslt_file1.cc

conf_xsltsheet.a: make-lib conf_xslt_file.o conf_xsltsheet.o
	./make-lib conf_xsltsheet conf_xslt_file.o

conf_xsltsheet.o: compile conf_xsltsheet.cc conf_xsltsheet.h conf_xslt_file.h \
conf_home.h
	./compile conf_xsltsheet.cc

conf_xsltsheet1.a: make-lib conf_xslt_file1.o conf_xsltsheet1.o
	./make-lib conf_xsltsheet1 conf_xslt_file1.o

conf_xsltsheet1.o: compile conf_xsltsheet1.cc conf_xsltsheet1.h \
conf_xslt_file1.h conf_home.h
	./compile conf_xsltsheet1.cc

cuimod.o: comp-so cuimod.h cuimod.h vqwww.inc cgicc.inc vq.inc \
conf_cntry_minl.h conf_city_minl.h valid.inc
	./comp-so cuimod.cc `cat vqwww.inc` `cat cgicc.inc` `cat vq.inc` \
	`cat valid.inc`

daemon.o: compile daemon.cc daemon.h vq.inc
	./compile daemon.cc `cat vq.inc`

freemail.o: comp-so freemail.cc cuimod.h vqwww.inc vq.inc cgicc.inc
	./comp-so freemail.cc `cat vqwww.inc` `cat vq.inc` `cat cgicc.inc`

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

install: load install.o conf_vq_uid.o conf_vq_gid.o
	./load install conf_vq_uid.o conf_vq_gid.o
	
install.o: compile install.cc conf_vq_uid.h conf_vq_gid.h conf_home.h
	./compile install.cc

int2cc: int2cc.o load
	./load int2cc

int2cc.o: compile int2cc.cc
	./compile int2cc.cc

libfreemail.so: make-so freemail.o cuimod.o cdbdaemon.o pgsql.lib vq.lib \
cgicc.lib conf_city_minl.o conf_cntry_minl.o valid.lib
	./make-so freemail freemail.o cuimod.o cdbdaemon.o conf_city_minl.o \
	conf_cntry_minl.o `cat pgsql.lib` \
	`cat vq.lib` `cat cgicc.lib` `cat valid.lib`

sabmain.o: compile sabmain.cc conf_xmlstart.h conf_xsltsheet.h sab.inc
	./compile sabmain.cc `cat sab.inc`

str2cc: str2cc.o load
	./load str2cc

str2cc.o: compile str2cc.cc
	./compile str2cc.cc

sstr2cc: sstr2cc.o load
	./load sstr2cc

sstr2cc.o: compile sstr2cc.cc
	./compile sstr2cc.cc

vqrescue: load vqrescue.o sabmain.o sab.lib conf_xmlstart.o \
conf_xsltsheet.a cgi.o cgicc.lib conf_user_ok.o \
conf_pass_ok.o conf_html_br.o html.o vq.lib valid.lib conf_pgsql.o pgsql.lib \
conf_notify.o cdbdaemon.o
	./load vqrescue sabmain.o `cat sab.lib` conf_xmlstart.o \
    conf_xsltsheet.a cgi.o `cat cgicc.lib` \
	conf_user_ok.o conf_pass_ok.o conf_html_br.o html.o conf_pgsql.o \
	conf_notify.o cdbdaemon.o \
	`cat vq.lib valid.lib pgsql.lib`

vqrescue.o: compile vqrescue.cc cgicc.inc conf_user_ok.h \
conf_pass_ok.h conf_html_br.h html.h vq.inc valid.inc conf_pgsql.h pgsql.inc \
conf_notify.h cdbdaemon.h conf_home.h
	./compile vqrescue.cc `cat cgicc.inc vq.inc valid.inc pgsql.inc`

vqsignup1: load vqsignup1.o sabmain.o sab.lib conf_xmlstart.o \
conf_xsltsheet.a cgi.o cgicc.lib conf_user_ok.o \
conf_pass_ok.o conf_html_br.o html.o vq.lib valid.lib conf_pgsql.o pgsql.lib \
conf_notify.o cdbdaemon.o conf_pass_minl.o conf_pass_maxl.o conf_user_minl.o \
conf_user_maxl.o conf_cntry_minl.o conf_city_minl.o
	./load vqsignup1 sabmain.o `cat sab.lib` conf_xmlstart.o \
    conf_xsltsheet.a cgi.o `cat cgicc.lib` \
	conf_user_ok.o conf_pass_ok.o conf_html_br.o html.o conf_pgsql.o \
	conf_notify.o cdbdaemon.o conf_pass_minl.o conf_pass_maxl.o conf_user_minl.o \
	conf_user_maxl.o conf_cntry_minl.o conf_city_minl.o \
	`cat vq.lib valid.lib pgsql.lib`

vqsignup1.o: compile vqsignup1.cc cgicc.inc conf_user_ok.h \
conf_pass_ok.h conf_html_br.h html.h vq.inc valid.inc conf_pgsql.h pgsql.inc \
conf_notify.h cdbdaemon.h conf_home.h conf_pass_minl.h conf_pass_maxl.h \
conf_user_minl.h conf_user_maxl.h conf_cntry_minl.h conf_city_minl.h
	./compile vqsignup1.cc `cat cgicc.inc vq.inc valid.inc pgsql.inc`

vqsignup1_notify: load vqsignup1_notify.o sab.lib conf_xmlstart1.o \
conf_xsltsheet1.a conf_pgsql.o html.o pgsql.lib vq.lib
	./load vqsignup1_notify conf_xmlstart1.o conf_xsltsheet1.a conf_pgsql.o \
	html.o `cat sab.lib pgsql.lib vq.lib` 

vqsignup1_notify.o: compile vqsignup1_notify.cc conf_xmlstart.h \
conf_xsltsheet.h conf_pgsql.h html.h sab.inc pgsql.inc vq.inc
	./compile vqsignup1_notify.cc `cat sab.inc pgsql.inc vq.inc`

vqsignup1d: load vqsignup1d.o conf_pgsql.o vq.lib pgsql.lib \
daemon.o 
	./load vqsignup1d conf_pgsql.o daemon.o \
	`cat vq.lib` `cat pgsql.lib`

vqsignup1d.o: compile vqsignup1d.cc conf_pgsql.cc vq.inc pgsql.inc conf_home.h \
daemon.h conf_home.h
	./compile vqsignup1d.cc `cat vq.inc` `cat pgsql.inc`

#####################
clean:
		rm -rf `cat TARGETS`
