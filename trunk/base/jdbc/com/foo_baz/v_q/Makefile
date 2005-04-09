BASE=../../../../..

.PHONY: all
.SUFFIXES: .class .java

all:

include $(BASE)/Makefile.common

all: POAHier.class JDBCLog.class

ilogger.class: iloggerOperations.class

iloggerOperations.class: except.class db_error.class null_error.class

except.class:
	$(JAVAC) -classpath . except.java exceptHelper.java

db_error.class:
	$(JAVAC) -classpath . db_error.java db_errorHelper.java

null_error.class:
	$(JAVAC) -classpath . null_error.java null_errorHelper.java

JDBCLog.class: ilogger.class

.java.class: 
	$(JAVAC) -classpath $(BASE)/corbautil/corbautil.jar:. $<

#idlj -fserver -td ../../.. -i ../../../.. -pkgTranslate vq com.foo_baz.v_q ../../../../auth.idl
#idlj -fserver -td ../../.. -i ../../../.. -pkgTranslate vq com.foo_baz.v_q ../../../../logger.idl
