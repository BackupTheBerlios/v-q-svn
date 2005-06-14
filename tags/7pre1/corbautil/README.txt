CORBA Utilities, version 2.1
----------------------------

This directory hierarchy contains a collection of CORBA utilities.
The directory structure is as follows:

	o cxx      contains C++ source code.
	o java     contains Java source code.
	o doc      contains documentation in the PDF format.
	o javadoc  contains Javadoc documentation for the Java utilities.
	o bin      contains some Orbix-specific utilities.

The utilities are known to work with the following CORBA implementations:

	o Orbix/C++  5.1, 6.0 and 6.1
	o Orbix/Java 5.1, 6.0 and 6.1
	o ORBacus/C++  4.1.x
	o ORBacus/Java 4.1.x
	o TAO (C++) 1.3 and 1.4
	o omniORB (C++) 4.0.3

It should be easy for users to port the utilities to support other
implementations of CORBA. If you decide to port the utilities to
another ORB then read the "Portability of C++ CORBA Applications" and
"Creation of POA Hierarchies Made Simple" chapters of the documentation
for helpful advice on porting.

Currently, only a Windows-style Makefile is provided for compiling the
utilities. UNIX-style Makefiles may be added in the future if I ever
find the time. If you want to use the CORBA Utilities on UNIX then
please note the following:

	o If you want to use the Java version then there is no need to
	  compile the Java source code, because the "corbautil.jar"
	  file contains the pre-compiled code.

	o If you want to use the C++ version then please note that
	  most of the C++ source code is in the form of ".h" files
	  that do not need to be compiled. There are only two ".cxx"
	  files that you need to compile. They are:

		cxx/import_export/import_export.cxx
		cxx/PoaUtility/PoaUtility.cxx

	  Write a UNIX Makefile that compiles those files with your
	  favorite compiler/CORBA product and adds the files to a
	  library called "corbautil.a" or somesuch.



------------------------------------------------------------------------
C++
------------------------------------------------------------------------

To compile the C++ source code:

	o Edit the "Makefile.inc" file and uncomment the include
	  statement appropriate for the CORBA product that you are
	  using.

	o Edit the "Makefile.inc.<product>" file and follow the
	  instructions in the comments.

	o Go into the "cxx" directory and type "nmake". This will
	  compile the source code and produce a file called
	  "corbautil.lib" in the top-level directory.


------------------------------------------------------------------------
Java
------------------------------------------------------------------------

The Java source code has been pre-compiled and the result packaged into
"corbautil.jar" in the top-level directory. You should add this
.jar file into your CLASSPATH.

If you want to re-compile the Java source code then you should do the
following:

	o If you have ORBacus and/or Orbix on your computer then edit
	  "Makefile.inc.<product>" and follow the instructions in the
	  comments.

	o Go into the "java" directory.

	o Type "nmake <target>" where target is one of the following:
		portable	This compiles code that is portable to
				all CORBA vendor products.
		orbix		This compiles code that is specific to
				Orbix.
		orbacus		This compiles code that is specific to
				ORBacus.
		jar		This creates "corbautil.jar" in the
				top-level directory from all the
				compiled .class files.
		javadoc		This creates Javadoc documentation.
		all		This includes all the above targets.
		clean		This deletes .class files.



------------------------------------------------------------------------
Orbix
------------------------------------------------------------------------

To use the Orbix-specific command-line utilities:

	o Go into the "bin" directory.

	o Run the command "itadmin post_install_fixup.tcl". This
	  will fix up some "/full/path/to/files" entries in case
	  you have installed the CORBA Utilities package in a
	  non-default location.

	o Add the "bin" directory into your PATH environment variable.

	o If you are on UNIX instead of Windows then write (trivial)
	  UNIX shell scripts that correspond to the ".bat" files in the
	  bin directory.



------------------------------------------------------------------------
Queries and help
------------------------------------------------------------------------

The CORBA Utilities package is NOT an official IONA product, so please
do not send questions or bug reports to IONA's technical support team.
Instead, this software has been developed and is maintained by Ciaran
McHale, who is a principal consultant at IONA Technologies. If you have
a problem then I will try to help, subject to other work commitments.
You can contact me by email at: Ciaran.McHale@iona.com

