%ifarch ia64
# disable debuginfo on IA64
%define debug_package %{nil}
%endif

Summary: Virtual Qmail
Name: v-q
Version: VQ_VERSION
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
exec g++ -L. -shared -o "$main" \${1+"\$@"}
EOF

cat > load << EOF
#!/bin/sh
main="\$1"; shift
exec g++ -L. -o "$main" "$main".o \${1+"\$@"}
EOF

%build
make

%install
rm -rf $RPM_BUILD_ROOT

%pre
# Add the "apache" user
/usr/sbin/useradd -c "Apache" -u 48 \
	-s /sbin/nologin -r -d %{contentdir} apache 2> /dev/null || :

%triggerpostun -- apache < 2.0
/sbin/chkconfig --add httpd

%post
# Register the httpd service
/sbin/chkconfig --add httpd

%preun
if [ $1 = 0 ]; then
	/sbin/service httpd stop > /dev/null 2>&1
	/sbin/chkconfig --del httpd
fi

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

%doc ABOUT_APACHE README CHANGES LICENSE

%dir %{_sysconfdir}/httpd
%{_sysconfdir}/httpd/modules
%{_sysconfdir}/httpd/logs
%{_sysconfdir}/httpd/run
%dir %{_sysconfdir}/httpd/conf
%config(noreplace) %{_sysconfdir}/httpd/conf/*.conf
%config(noreplace) %{_sysconfdir}/httpd/conf/magic
%config(noreplace) %{_sysconfdir}/httpd/conf/mime.types

%config %{_sysconfdir}/logrotate.d/httpd
%config %{_sysconfdir}/rc.d/init.d/httpd

#%dir %{_sysconfdir}/httpd/conf

%{_bindir}/ab
%{_bindir}/ht*
%{_bindir}/logresolve
%{_sbindir}/httpd
%{_sbindir}/httpd.worker
%{_sbindir}/apachectl
%{_sbindir}/rotatelogs
%attr(4510,root,%{suexec_caller}) %{_sbindir}/suexec

%{_libdir}/libapr-0.so.*
%{_libdir}/libaprutil-0.so.*

%dir %{_libdir}/httpd
%dir %{_libdir}/httpd/modules
# everything but mod_ssl.so:
%{_libdir}/httpd/modules/mod_[a-r]*.so
%{_libdir}/httpd/modules/mod_s[petu]*.so
%{_libdir}/httpd/modules/mod_[t-z]*.so

%dir %{contentdir}
%dir %{contentdir}/cgi-bin
%dir %{contentdir}/html
%dir %{contentdir}/icons
%dir %{contentdir}/error
%dir %{contentdir}/error/include
%{contentdir}/icons/*
%config(noreplace) %{contentdir}/error/*.var
%config(noreplace) %{contentdir}/error/include/*.html

%attr(0700,root,root) %dir %{_localstatedir}/log/httpd

%attr(0700,apache,apache) %dir %{_localstatedir}/lib/dav

%{_mandir}/man1/*

%{_mandir}/man8/rotatelogs*
%{_mandir}/man8/suexec*

%changelog
* Fri Mar 25 2005 Pawel Niewiadomski <new@foo-baz.com> 6
- First version.
