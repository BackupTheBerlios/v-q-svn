/*
Copyright (c) 2002,2003 Pawel Niewiadomski
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
#include "cdaemonlog.h"
#include "conf_home.h"
#include "fd.h"
#include "fdstr.h"
#include "lower.h"
#include "sys.h"

#include <new>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>

using namespace std;
using namespace vq;

/**
 *
 */
clogger * clogger::alloc() {
	return new cdaemonlog();
}

/**
 *
 */
cdaemonlog::cdaemonlog() {
	int curdirfd = open(".",O_RDONLY);
	if(curdirfd==-1) 
			throw runtime_error((string)"cdaemonlog: open: "+conf_home
					+": "+strerror(errno));
	try {
			if(!sig_pipe_ign())
					throw runtime_error((string)"cdaemonlog: sig_pipe_ign: "+strerror(errno));
			if(chdir((conf_home+"/sockets").c_str()))
					throw runtime_error((string)"cdaemonlog: chdir: "
							+conf_home+"/sockets: "+strerror(errno));
			if( (cso=socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
					throw runtime_error((string)"cdaemonlog: socket: "+strerror(errno));
			cun.sun_family = AF_UNIX;
			strcpy(cun.sun_path, "daemonlog");
			if(connect(cso, (struct sockaddr*) &cun, SUN_LEN(&cun)))
					throw runtime_error((string)"cdaemonlog: connect: daemonauth: "+strerror(errno));
			if( fchdir(curdirfd) )
					throw runtime_error((string)"cdaemonlog: fchdir: "+conf_home
							+": "+strerror(errno));
			close(curdirfd);
	} catch(...) {
			close(curdirfd);
			throw;
	}
}

cdaemonlog::~cdaemonlog() {
	close(cso);
}

/**
 *
 */
uint8_t cdaemonlog::write( result res, const string &msg ) {
	try {
			cmd = cmd_write;
			if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
				&& fdwrstr(cso, dom) != -1
				&& fdwrstr(cso, log) != -1
				&& fdwrite(cso, &ser, sizeof(ser)) != -1
				&& fdwrite(cso, &res, sizeof(res)) != -1
				&& fdwrstr(cso, ip) != -1
				&& fdwrstr(cso, msg) != -1
				&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
				&& resp == 'K' ) return lr(err_no);
	} catch(...) {
	}
	return lr(err_temp);
}

/**
 * \param hist logging history
 * \param start if != 0 means start getting entries from this position
 * \param cnt if != 0 means to get only cnt entries
 */
uint8_t cdaemonlog::log_read_dom_log( vector<entry_type> & hist, size_type start, 
		size_type cnt ) {
	cmd = cmd_log_read_dom_log;
	return read_common(hist, start, cnt);
}

/**
 * \param hist logging history
 * \param start if != 0 means start getting entries from this position
 * \param cnt if != 0 means to get only cnt entries
 */
uint8_t cdaemonlog::dom_log_read_log( vector<entry_type> & hist, size_type start, 
		size_type cnt ) {
	cmd = cmd_dom_log_read_log;
	return read_common(hist, start, cnt);
}

/**
 * \param hist logging history
 * \param start if != 0 means start getting entries from this position
 * \param cnt if != 0 means to get only cnt entries
 */
uint8_t cdaemonlog::read_common( vector<entry_type> & hist, size_type start, 
		size_type cnt ) {
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, dom) != -1
		&& fdwrstr(cso, log) != -1
		&& fdwrite(cso, &start, sizeof(start)) != -1
		&& fdwrite(cso, &cnt, sizeof(cnt)) != -1
		&& fdread(cso, &resp, sizeof(resp)) != -1 ) {
			hist.clear();
			
			switch(resp) {
			case 'E': return lr(err_temp);
			case 'Z': return lr(err_empty);
			}

			uint32_t hcnt;
			if( fdread(cso, &hcnt, sizeof(hcnt)) == -1 ) return lr(err_rd);
			if( hcnt-- ) {
					entry_type ent;
					do {
							if( fdrdstr(cso, ent.id) == -1 
								|| fdread(cso, &ent.ser, sizeof(ent.ser)) == -1 
								|| fdread(cso, &ent.res, sizeof(ent.res)) == -1
								|| fdrdstr(cso, ent.ip) == -1 
								|| fdrdstr(cso, ent.msg) == -1
								|| fdrdstr(cso, ent.time) == -1 )
									return lr(err_rd);
							hist.push_back(ent);
					} while( hcnt -- );
			}
			return lr(err_no);
	}
	return lr(err_wr);
}

/**
 * Count entries for user\@domain in log
 */
uint8_t cdaemonlog::log_cnt_dom_log( size_type & cnt ) {
	cmd = cmd_log_cnt_dom_log;
	return cnt_common(cnt);
}

/**
 * Count entries for user\@domain in domain_log
 */
uint8_t cdaemonlog::dom_log_cnt_log( size_type & cnt ) {
	cmd = cmd_dom_log_cnt_log;
	return cnt_common(cnt);
}

/**
 */
uint8_t cdaemonlog::cnt_common( size_type &cnt ) {
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, dom) != -1
		&& fdwrstr(cso, log) != -1
		&& fdread(cso, &resp, sizeof(resp)) != -1
		&& 'F' == resp ) {
			
			if( fdread(cso, &cnt, sizeof(cnt)) == -1 ) return lr(err_rd);
			return lr(err_no);
	}
	return lr(resp == 'E' ? err_temp : err_wr);
}

