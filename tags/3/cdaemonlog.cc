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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>

#include "cdaemonlog.h"
#include "conf_vq.h"
#include "fd.h"
#include "fdstr.h"
#include "lower.h"

using namespace std;

/**
 *
 */
void log_init(clogger **l) {
	*l = new cdaemonlog();
}

/**
 *
 */
cdaemonlog::cdaemonlog() throw(runtime_error) {
	int curdirfd = open(".",O_RDONLY);
	if(curdirfd==-1) 
			throw runtime_error((string)"cdaemonlog: open: "+conf_vq
					+": "+strerror(errno));
	if(chdir((conf_vq+"/sockets").c_str()))
			throw runtime_error((string)"cdaemonlog: chdir: "
					+conf_vq+"/sockets: "+strerror(errno));
	if( (cso=socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
			throw runtime_error((string)"cdaemonlog: socket: "+strerror(errno));
	cun.sun_family = AF_UNIX;
	strcpy(cun.sun_path, "daemonlog");
	if(connect(cso, (struct sockaddr*) &cun, SUN_LEN(&cun)))
			throw runtime_error((string)"cdaemonlog: connect: daemonauth: "+strerror(errno));
	if( fchdir(curdirfd) )
			throw runtime_error((string)"cdaemonlog: fchdir: "+conf_vq
					+": "+strerror(errno));
}

cdaemonlog::~cdaemonlog() {
	close(cso);
}

void cdaemonlog::e(const string &s) throw()
{
	send(cmd_e, s);
}
void cdaemonlog::estr(const string &s) throw()
{
	send(cmd_estr, s, strerror(errno));
}
void cdaemonlog::w(const string &s) throw()
{
	send(cmd_w, s);
}
void cdaemonlog::wstr(const string &s) throw()
{
	send(cmd_wstr, s, strerror(errno));
}
void cdaemonlog::n(const string &s) throw()
{
	send(cmd_n, s);
}
void cdaemonlog::nstr(const string &s) throw()
{
	send(cmd_nstr, s, strerror(errno));
}
void cdaemonlog::d(const string &s) throw()
{
	send(cmd_d, s);
}
void cdaemonlog::dstr(const string &s) throw()
{
	send(cmd_dstr, s, strerror(errno));
}
///////////////////
void cdaemonlog::ae(const string &s, const string &d,const string &u) throw()
{
	asend(cmd_ae, d, u, s);
}
void cdaemonlog::aestr(const string &s, const string &d,const string &u) throw()
{
	asend(cmd_aestr, d, u, s, strerror(errno));
}
void cdaemonlog::aw(const string &s, const string &d,const string &u) throw()
{
	asend(cmd_aw, d, u, s);
}
void cdaemonlog::awstr(const string &s, const string &d,const string &u) throw()
{
	asend(cmd_awstr, d, u, s, strerror(errno));
}
void cdaemonlog::an(const string &s, const string &d,const string &u) throw()
{
	asend(cmd_an, d, u, s);
}
void cdaemonlog::anstr(const string &s, const string &d,const string &u) throw()
{
	asend(cmd_anstr, d, u, s, strerror(errno));
}
void cdaemonlog::ad(const string &s, const string &d,const string &u) throw()
{
	asend(cmd_ad, d, u, s);
}
void cdaemonlog::adstr(const string &s, const string &d,const string &u) throw()
{
	asend(cmd_adstr, d, u, s, strerror(errno));
}

/**
 *
 */
uint8_t cdaemonlog::send(uint8_t cmd, const string &s ) {
	char resp;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, s) != -1
		&& fdwrstr(cso, clip) != -1
		&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
		&& resp == 'K' ) return 0;
	errmsg = (string)"send: "+strerror(errno);
	return 1;
}

/**
 *
 */
uint8_t cdaemonlog::send(uint8_t cmd, const string &s, const string & s1 ) {
	char resp;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, s) != -1
		&& fdwrstr(cso, s1) != -1
		&& fdwrstr(cso, clip) != -1
		&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
		&& resp == 'K' ) return 0;
	errmsg = (string)"send:: "+strerror(errno);
	return 1;
}

/**
 *
 */
uint8_t cdaemonlog::asend(uint8_t cmd, const string &d, const string &u, 
		const string &s ) {
	char resp;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, d) != -1
		&& fdwrstr(cso, u) != -1
		&& fdwrstr(cso, s) != -1
		&& fdwrstr(cso, clip) != -1
		&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
		&& resp == 'K' ) return 0;
	errmsg = (string)"asend: "+strerror(errno);
	return 1;
}

/**
 *
 */
uint8_t cdaemonlog::asend(uint8_t cmd, const string &d, const string &u,
		const string &s, const string & s1 ) {
	char resp;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, d) != -1
		&& fdwrstr(cso, u) != -1
		&& fdwrstr(cso, s) != -1
		&& fdwrstr(cso, s1) != -1
		&& fdwrstr(cso, clip) != -1
		&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
		&& resp == 'K' ) return 0;
	errmsg = (string)"asend:: "+strerror(errno);
	return 1;
}
