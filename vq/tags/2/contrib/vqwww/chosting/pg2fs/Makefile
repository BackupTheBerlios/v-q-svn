it: pg2fs

clean:
	rm -rf `cat TARGETS`

conf_dnsc_gid.cc: getgid int2cc conf/dnsc_group
	head -n 1 conf/dnsc_group | ./getgid | ./int2cc dnsc_gid > conf_dnsc_gid.cc
	
conf_dnsc_gid.o: compile conf_dnsc_gid.cc conf_dnsc_gid.h
	./compile conf_dnsc_gid.cc

conf_dnsc_mode.cc: int2cc conf/dnsc_mode
	head -n 1 conf/dnsc_mode | ./int2cc dnsc_mode > conf_dnsc_mode.cc

conf_dnsc_mode.o: compile conf_dnsc_mode.h conf_dnsc_mode.cc
	./compile conf_dnsc_mode.cc

conf_dnsc_restart.cc: str2cc conf/dnsc_restart
	head -n 1 conf/dnsc_restart | ./str2cc dnsc_restart > conf_dnsc_restart.cc

conf_dnsc_restart.o: compile conf_dnsc_restart.h conf_dnsc_restart.cc
	./compile conf_dnsc_restart.cc

conf_dnsc_uid.cc: getuid int2cc conf/dnsc_user
	head -n 1 conf/dnsc_user | ./getuid | ./int2cc dnsc_uid > conf_dnsc_uid.cc
	
conf_dnsc_uid.o: compile conf_dnsc_uid.cc conf_dnsc_uid.h
	./compile conf_dnsc_uid.cc

conf_dnsc_root.cc: str2cc conf/dnsc_root
	head -n 1 conf/dnsc_root | ./str2cc dnsc_root a > conf_dnsc_root.cc
	
conf_dnsc_root.o: compile conf_dnsc_root.h conf_dnsc_root.cc
	./compile conf_dnsc_root.cc

conf_tdns_gid.cc: getgid int2cc conf/tdns_group
	head -n 1 conf/tdns_group | ./getgid | ./int2cc tdns_gid > conf_tdns_gid.cc
	
conf_tdns_gid.o: compile conf_tdns_gid.cc conf_tdns_gid.h
	./compile conf_tdns_gid.cc

conf_tdns_make.cc: str2cc conf/tdns_make
	head -n 1 conf/tdns_make | ./str2cc tdns_make > conf_tdns_make.cc

conf_tdns_make.o: compile conf_tdns_make.h conf_tdns_make.cc
	./compile conf_tdns_make.cc

conf_tdns_mode.cc: int2cc conf/tdns_mode
	head -n 1 conf/tdns_mode | ./int2cc tdns_mode > conf_tdns_mode.cc

conf_tdns_mode.o: compile conf_tdns_mode.h conf_tdns_mode.cc
	./compile conf_tdns_mode.cc

conf_tdns_uid.cc: getuid int2cc conf/tdns_user
	head -n 1 conf/tdns_user | ./getuid | ./int2cc tdns_uid > conf_tdns_uid.cc
	
conf_tdns_uid.o: compile conf_tdns_uid.cc conf_tdns_uid.h
	./compile conf_tdns_uid.cc

conf_tdns_root.cc: str2cc conf/tdns_root
	head -n 1 conf/tdns_root | ./str2cc tdns_root a > conf_tdns_root.cc
	
conf_tdns_root.o: compile conf_tdns_root.h conf_tdns_root.cc
	./compile conf_tdns_root.cc

getgid: load getgid.o
	./load getgid

getgid.o: compile getgid.cc
	./compile getgid.cc

getuid: load getuid.o
	./load getuid

getuid.o: compile getuid.cc
	./compile getuid.cc

hasflock.h: tryflock.cc compile load
	( ( ./compile tryflock.cc && ./load tryflock ) >/dev/null 2>&1 \
    && echo \#define HASFLOCK 1 || exit 0 ) > hasflock.h
	rm -f tryflock.o tryflock

int2cc: load int2cc.o
	./load int2cc

int2cc.o: compile int2cc.cc
	./compile int2cc.cc

lock.o: compile lock.cc lock.h hasflock.h
	./compile lock.cc

pg2fs: load pg2fs.o conf_tdns_root.o conf_tdns_uid.o conf_tdns_gid.o lock.o \
conf_tdns_mode.o conf_dnsc_root.o conf_dnsc_uid.o conf_dnsc_gid.o \
conf_dnsc_mode.o conf_dnsc_restart.o conf_tdns_make.o
	./load pg2fs conf_tdns_root.o conf_tdns_uid.o conf_tdns_gid.o lock.o \
	conf_tdns_mode.o conf_dnsc_root.o conf_dnsc_uid.o conf_dnsc_gid.o \
	conf_dnsc_mode.o conf_dnsc_restart.o conf_tdns_make.o

pg2fs.o: compile pg2fs.cc conf_tdns_root.h conf_tdns_uid.h conf_tdns_gid.h \
lock.h conf_tdns_mode.h conf_dnsc_root.h conf_dnsc_uid.h conf_dnsc_gid.h \
conf_dnsc_mode.h conf_dnsc_restart.h conf_tdns_make.h
	./compile pg2fs.cc

str2cc: load str2cc.o
	./load str2cc

str2cc.o: compile str2cc.cc
	./compile str2cc.cc

