/*
Copyright (c) 2002 Pawel Niewiadomski
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the Pawel Niewiadomski,
   and other contributors.
4. Neither the name of Pawel Niewiadomski nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY PAWEL NIEWIADOMSKI AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL PAWEL NIEWIADOMSKI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

#include <new>
#include <syslog.h>
#include <string>
#include <cerrno>

#include "csyslog.h"

using namespace std;

csyslog::csyslog(const char *prep) throw()
{
	openlog(prep, LOG_PID, LOG_MAIL);
}

void csyslog::e(const string &s) throw()
{
	syslog(LOG_ERR, "%s", s.c_str());
}
void csyslog::estr(const string &s) throw()
{
	syslog(LOG_ERR, "%s: %s", s.c_str(), strerror(errno));
}
void csyslog::w(const string &s) throw()
{
	syslog(LOG_WARNING, "%s", s.c_str());
}
void csyslog::wstr(const string &s) throw()
{
	syslog(LOG_WARNING,"%s: %s", s.c_str(), strerror(errno));
}
void csyslog::n(const string &s) throw()
{
	syslog(LOG_NOTICE,"%s", s.c_str());
}
void csyslog::nstr(const string &s) throw()
{
	syslog(LOG_NOTICE, "%s: %s", s.c_str(), strerror(errno));
}
void csyslog::d(const string &s) throw()
{
	syslog(LOG_DEBUG,"%s", s.c_str());
}
void csyslog::dstr(const string &s) throw()
{
	syslog(LOG_DEBUG,"%s: %s", s.c_str(), strerror(errno));
}
///////////////////
void csyslog::ae(const string &s, const string &d,const string &u) throw()
{
	syslog(LOG_ERR, "A: D:%s;U:%s: %s", d.c_str(), u.c_str(), s.c_str());
}
void csyslog::aestr(const string &s, const string &d,const string &u) throw()
{
	syslog(LOG_ERR, "A: D:%s;U:%s: %s: %s", d.c_str(), u.c_str(), s.c_str(), strerror(errno));
}
void csyslog::aw(const string &s, const string &d,const string &u) throw()
{
	syslog(LOG_WARNING, "A: D:%s;U:%s: %s", d.c_str(), u.c_str(), s.c_str());
}
void csyslog::awstr(const string &s, const string &d,const string &u) throw()
{
	syslog(LOG_WARNING,"A: D:%s;U:%s: %s: %s", d.c_str(), u.c_str(), s.c_str(), strerror(errno));
}
void csyslog::an(const string &s, const string &d,const string &u) throw()
{
	syslog(LOG_NOTICE,"A: D:%s;U:%s: %s", d.c_str(), u.c_str(), s.c_str());
}
void csyslog::anstr(const string &s, const string &d,const string &u) throw()
{
	syslog(LOG_NOTICE, "A: D:%s;U:%s: %s: %s", d.c_str(), u.c_str(), s.c_str(), strerror(errno));
}
void csyslog::ad(const string &s, const string &d,const string &u) throw()
{
	syslog(LOG_DEBUG,"A: D:%s;U:%s: %s", d.c_str(), u.c_str(), s.c_str());
}
void csyslog::adstr(const string &s, const string &d,const string &u) throw()
{
	syslog(LOG_DEBUG,"A: D:%s;U:%s: %s: %s", d.c_str(), u.c_str(), s.c_str(), strerror(errno));
}

void log_init(clogger **l) {
	*l = new csyslog("VQ");
}
