!include "../Makefile.inc.orbix"
!include "../Makefile.inc.orbacus"

PORTABLE_SRC=	\
		com/iona/corbautil/PoaUtilityException.java	\
		com/iona/corbautil/LabelledPOAManager.java	\
		com/iona/corbautil/PoaUtility.java	\
		com/iona/corbautil/PoaUtilityPortableImpl.java \
		com/iona/corbautil/ImportExport.java \
		com/iona/corbautil/ImportExportAlgorithm.java \
		com/iona/corbautil/ImportExportException.java \
		com/iona/corbautil/ImportExportExampleAlgorithm.java

ORBIX_SRC=	\
		com/iona/corbautil/LabelledOrbixWorkQueue.java	\
		com/iona/corbautil/PoaUtilityOrbixImpl.java

ORBACUS_SRC=	\
		com/iona/corbautil/PoaUtilityOrbacusImpl.java

all:	portable orbacus orbix jar javadoc

jar:
	jar cvf ..\corbautil.jar com/iona/corbautil/*.class

orbix:	
	it_javac $(ORBIX_SRC)

portable:
	it_javac $(PORTABLE_SRC)

orbacus:
	javac -bootclasspath $(OB_JAR);$(JAVA_HOME)\jre\lib\rt.jar \
				$(ORBACUS_SRC)

javadoc:
	-mkdir ..\javadoc
	javadoc -d ..\javadoc \
		-classpath "$(OB_JAR);$(ART_JAR_FILE);$(CLASSPATH)" \
		-sourcepath . \
		com/iona/corbautil/*.java

clean:
	-del com\iona\corbautil\*.class
