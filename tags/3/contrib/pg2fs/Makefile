it: auto pg2fs

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

clean:
	rm -rf `cat TARGETS`

cconf.o: compile cconf.h cconf.cc
	./compile cconf.cc

auto/dnsc_gid.cc: getgid ac_int conf/dnsc_group
	./getgid `head -n 1 conf/dnsc_group` | ./ac_int pg2fs dnsc_gid 1 cgidconf
	
auto/dnsc_gid.o: compile auto/dnsc_gid.cc cconf.h
	cd auto && ../compile dnsc_gid.cc

auto/dnsc_mode.cc: ac_int conf/dnsc_mode
	head -n 1 conf/dnsc_mode | ./ac_int pg2fs dnsc_mode

auto/dnsc_mode.o: compile cconf.h auto/dnsc_mode.cc
	cd auto && ../compile dnsc_mode.cc

auto/dnsc_restart.cc: ac_str conf/dnsc_restart
	head -n 1 conf/dnsc_restart | ./ac_str pg2fs dnsc_restart a

auto/dnsc_restart.o: compile cconf.h auto/dnsc_restart.cc
	cd auto && ../compile dnsc_restart.cc

auto/dnsc_uid.cc: getuid ac_int conf/dnsc_user
	./getuid `head -n 1 conf/dnsc_user` | ./ac_int pg2fs dnsc_uid 1 cuidconf
	
auto/dnsc_uid.o: compile auto/dnsc_uid.cc cconf.h
	cd auto && ../compile dnsc_uid.cc

auto/dnsc_root.cc: ac_str conf/dnsc_root
	head -n 1 conf/dnsc_root | ./ac_str pg2fs dnsc_root a
	
auto/dnsc_root.o: compile cconf.h auto/dnsc_root.cc
	cd auto && ../compile dnsc_root.cc

auto/tdns_gid.cc: getgid ac_int conf/tdns_group
	./getgid `head -n 1 conf/tdns_group` | ./ac_int pg2fs tdns_gid 1 cgidconf
	
auto/tdns_gid.o: compile auto/tdns_gid.cc cconf.h
	cd auto && ../compile tdns_gid.cc

auto/tdns_make.cc: ac_str conf/tdns_make
	head -n 1 conf/tdns_make | ./ac_str pg2fs tdns_make a

auto/tdns_make.o: compile cconf.h auto/tdns_make.cc
	cd auto && ../compile tdns_make.cc

auto/tdns_mode.cc: ac_int conf/tdns_mode
	head -n 1 conf/tdns_mode | ./ac_int pg2fs tdns_mode

auto/tdns_mode.o: compile cconf.h auto/tdns_mode.cc
	cd auto && ../compile tdns_mode.cc

auto/tdns_uid.cc: getuid ac_int conf/tdns_user
	./getuid `head -n 1 conf/tdns_user` | ./ac_int pg2fs tdns_uid 1 cuidconf
	
auto/tdns_uid.o: compile auto/tdns_uid.cc cconf.h
	cd auto && ../compile tdns_uid.cc

auto/tdns_root.cc: ac_str conf/tdns_root
	head -n 1 conf/tdns_root | ./ac_str pg2fs tdns_root a
	
auto/tdns_root.o: compile cconf.h auto/tdns_root.cc
	cd auto && ../compile tdns_root.cc

conf_home.cc: str2cc conf/home
	./str2cc pg2fs "`head -n 1 conf/home`" home a > conf_home.cc

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

auto/hasflock.h: try/flock.cc compile load
	( ( cd try && ../compile flock.cc && ../load flock ) >/dev/null 2>&1 \
    && echo \#define HASFLOCK 1 || exit 0 ) > auto/hasflock.h
	rm -f try/flock.o try/flock

int2cc: load int2cc.o
	./load int2cc

int2cc.o: compile int2cc.cc
	./compile int2cc.cc

lock.o: compile lock.cc lock.h auto/hasflock.h
	./compile lock.cc

pg2fs: load pg2fs.o lock.o pg2fs_conf.a
	./load pg2fs lock.o pg2fs_conf.a

pg2fs.o: compile pg2fs.cc lock.h pg2fs_conf.h cconf.h
	./compile pg2fs.cc

pg2fs_conf.a: make-lib auto/dnsc_gid.o auto/dnsc_mode.o auto/dnsc_restart.o \
auto/dnsc_root.o auto/dnsc_uid.o auto/tdns_gid.o \
auto/tdns_make.o auto/tdns_mode.o auto/tdns_root.o auto/tdns_uid.o cconf.o
	./make-lib pg2fs_conf.a auto/dnsc_gid.o auto/dnsc_mode.o \
	auto/dnsc_restart.o auto/dnsc_root.o auto/dnsc_uid.o auto/tdns_gid.o \
	auto/tdns_make.o auto/tdns_mode.o auto/tdns_root.o auto/tdns_uid.o cconf.o

str2cc: load str2cc.o
	./load str2cc

str2cc.o: compile str2cc.cc
	./compile str2cc.cc

