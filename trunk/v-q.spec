%define datadir /var/vq

%ifarch ia64
# disable debuginfo on IA64
%define debug_package %{nil}
%endif

Summary: Virtual Qmail
Name: v-q
Version: @VERSION@
Release: 1
URL: http://v-q.foo-baz.com/
Vendor: http://foo-baz.com/
Packager: Pawel Niewiadomski <new@foo-baz.com>
Source0: %{name}-%{version}.tar.gz
License: GPL
Group: System Environment/Daemons
BuildRoot: %{_tmppath}/%{name}-root
BuildPrereq: findutils, perl, boost-devel >= 1.31.0, omniORB-devel >= 4.0.5-1
Prereq: /sbin/chkconfig, /bin/mktemp, /bin/rm, /bin/mv, /usr/sbin/useradd
Prereq: sh-utils, textutils, boost >= 1.31.0
Prereq: omniORB >= 4.0.5-1

%description
Virtual Qmail

%prep
%setup -q
echo "-I/usr/include" > boost.inc
echo "-L/usr/lib" > boost.lib
echo "-lboost_date_time" > boost_date_time.lib
echo "-lboost_regex" > boost_regex.lib
echo "-lboost_thread" > boost_thread.lib
echo "-lboost_unit_test_framework" > boost_unit_test.lib
echo "-DP_USE_OMNIORB -I/usr/include" > corba.inc
echo "-L/usr/lib -lCOS4 -lCOSDynamic4 -lomniORB4 -lssl -lcrypto -lpthread" > corba.lib

echo '#!/bin/sh' > idl
echo 'exec omniidl -bcxx -Wbh=.hpp -Wbs=.cc $@' >> idl
chmod u+x idl

cat > compile << EOF
#!/bin/sh
exec g++ $RPM_OPT_FLAGS -D_REENTRANT -ftemplate-depth-40 -pipe -g -Wall -c \${1+"\$@"}
EOF

cat > comp-so << EOF
#!/bin/sh
exec g++ $RPM_OPT_FLAGS -D_REENTRANT -g -ftemplate-depth-40 -fpic -DPIC -pipe -Wall -c \${1+"\$@"}
EOF

cat > make-lib << EOF
#!/bin/sh
exec ar cr \${1+"\$@"}
EOF

cat > make-so << EOF
#!/bin/sh
main="\$1"; shift
exec g++ -L. -shared -o "\$main" \${1+"\$@"}
EOF

cat > load << EOF
#!/bin/sh
main="\$1"; shift
exec g++ -L. -o "\$main" "\$main".o \${1+"\$@"}
EOF

%build
make

%install
rm -rf $RPM_BUILD_ROOT
make PREFIX=$RPM_BUILD_ROOT/usr install

%pre
# Add the "_vq" user
/usr/sbin/useradd -c "Virtual Qmail" -u 111 \
	-s /sbin/nologin -r -d %{datadir} _vq 2> /dev/null || :

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

%doc INSTALL VERSION README README.pl_PL doc/en.html doc/pl.html

%dir %{_sysconfdir}/ivq
%dir %{_sysconfdir}/iauth
%dir %{_sysconfdir}/ilogger

%{_bindir}/*
#%attr(4510,root,%{suexec_caller}) %{_sbindir}/suexec

%dir %{_libdir}/vq
%{_libdir}/vq/*

%attr(0700,_vq,_vq) %dir %{_localstatedir}/vq

%changelog
* Fri Mar 25 2005 Pawel Niewiadomski <new@foo-baz.com> 6
- First version.
