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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <ctime>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

#include "cdaemonlog.h"
#include "conf_home.h"
#include "vq_conf.h"
#include "sig.h"
#include "fd.h"
#include "fdstr.h"
#include "util.h"
#include "lock.h"
#include "pgsqlcommon.h"
#include "sys.h"
#include "daemon.h"

using namespace std;
using namespace vq;

// socket
int slock;
// pgsql
pqxx::Connection *pg = NULL;

// child
string msg, dom, login, ip;
cdaemonlog::result res;
cdaemonlog::service ser;
string sser, sres;

/**
@exception runtime_error if it can't create socket
*/
void setup()
{
	umask(0);
	if(!sig_pipeign() || !sig_chld_nocldwait() ) 
			throw runtime_error((string)"setup: can't set signals: "+strerror(errno));
	if(chdir((conf_home+"/sockets").c_str()))
			throw runtime_error((string)"setup: chdir: "
					+conf_home+"/sockets: "+strerror(errno));
	slock = open("daemonlog.lock", O_RDONLY | O_NONBLOCK | O_CREAT, 0666 );
	if(slock==-1)
			throw runtime_error((string)"setup: open: daemonlog.lock: "+strerror(errno));
	if(lock_exnb(slock)) {
			if( errno == EWOULDBLOCK )
					throw runtime_error((string)"setup: daemonlog.lock is already locked");
			else throw runtime_error((string)"setup: daemonlog.lock: lock: "+strerror(errno));
	}
	if(unlink("daemonlog")==-1 && errno != ENOENT )
			throw runtime_error((string)"setup: unlink: daemonlog: "+strerror(errno));
	if( (sso=socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
			throw runtime_error((string)"setup: socket: "+strerror(errno));
	struct sockaddr_un sun;
	sun.sun_family = AF_UNIX;
	strcpy(sun.sun_path, "daemonlog");
	if(bind(sso, (struct sockaddr*) &sun, SUN_LEN(&sun)))
			throw runtime_error((string)"setup: bind: daemonlog: "+strerror(errno));
	if( !(pg = new pqxx::Connection(ac_pgsql.val_str().c_str())) ) 
			throw runtime_error((string)"setup: no memory"); 
	if( listen(sso, 5) == -1 )
			throw runtime_error((string)"setup: listen: "+strerror(errno));
}

/**
 *
 */
string int2str( int i ) {
	ostringstream os;
	os<<i;
	return ! os ? "0" : os.str();
}

/**
 *
 */
void write( const string &log, 
		const string &ser, const string &res, 
		const string &ip, const string &msg ) {

	pqxx::Result x(pqxx::NonTransaction(*pg).Exec(
		"INSERT INTO log (LOGIN,SERVICE,RESULT,IP,MSG)"
		" VALUES("+pqxx::Quote(log, false)+","
		+pqxx::Quote(ser, false)+","+pqxx::Quote(res, false)+","
		+pqxx::Quote(ip, false)+","+pqxx::Quote(msg, false)+")"));
}

/**
 *
 */
void write( const string & dom, const string &log, 
		const string &ser, const string &res, 
		const string &ip, const string &msg ) {

	pqxx::Result x(pqxx::NonTransaction(*pg).Exec(
		(string)"INSERT INTO log (DOMAIN,LOGIN,SERVICE,RESULT,IP,MSG)"
		" VALUES("+pqxx::Quote(dom, false)+","+pqxx::Quote(log, false)+","
		+pqxx::Quote(ser, false)+","+pqxx::Quote(res, false)+","
		+pqxx::Quote(ip, false)+","+pqxx::Quote(msg, false)+")"));
}

/**
 *
 */
void write_dom( const string & dom, const string &log, 
		const string &ser, const string &res, 
		const string &ip, const string &msg ) {

	string tb(dom);
	str2tb(tb);
	
	pqxx::Result x(pqxx::NonTransaction(*pg).Exec(
		(string)"INSERT INTO \""+tb+"_log\" (LOGIN,SERVICE,RESULT,IP,MSG) "
		"VALUES("+pqxx::Quote(log, false)+","+pqxx::Quote(ser, false)+","
		+pqxx::Quote(res, false)+","+pqxx::Quote(ip, false)+","+pqxx::Quote(msg, false)+")"));
}	

/**
 * write information in database 
 */
void cmd_write() {
	if( fdrdstr(cso, dom) != -1
		&& fdrdstr(cso, login) != -1
		&& fdread(cso, &ser, sizeof(ser)) != -1
		&& fdread(cso, &res, sizeof(res)) != -1
		&& fdrdstr(cso, ip) != -1 
		&& fdrdstr(cso, msg) != -1 ) {
			sres = int2str( static_cast<int>(res) );
			sser = int2str( static_cast<int>(ser) );

			if( dom.empty() ) {
					write(login, sser, sres, ip, msg);
			} else {
					alias2dom(*pg, dom);
				
					try { 
						write_dom(dom, login, sser, sres, ip, msg);
					} catch(...) {
						write(dom, login, sser, sres, ip, msg);
					}
			}
			if(fdwrite(cso,&"K",1) != 1) throw wr_error();
	} else 
			throw rd_error();
}

/**
 *
 */
pqxx::Result dom_log_read_log(const string &dom, const string &log, 
		cdaemonlog::size_type start, cdaemonlog::size_type cnt ) {
	string tb(dom);
	str2tb(tb);

	string qr("SELECT id,time,service,msg,ip,result FROM \""+tb+"_log\""
			" WHERE login="+pqxx::Quote(log,false)
			+" ORDER BY time DESC");
	
	ostringstream os;
	if( cnt ) {
			os.str("");
			os<<cnt;
			qr += " LIMIT "+os.str();
	}
	if( start ) {
			os.str("");
			os<<start;
			qr+= " OFFSET "+os.str();
	}

	return pqxx::Result(pqxx::NonTransaction(*pg).Exec(qr));
}

/**
 *
 */
pqxx::Result log_read_dom_log(const string &dom, const string &log, 
		cdaemonlog::size_type start, cdaemonlog::size_type cnt ) {
	string tb(dom);
	str2tb(tb);

	string qr("SELECT id,time,service,msg,ip,result FROM log WHERE domain="
		+pqxx::Quote(dom,false)
		+" AND login="+pqxx::Quote(log,false)
		+" ORDER BY time DESC");
	
	ostringstream os;
	if( cnt ) {
			os.str("");
			os<<cnt;
			qr += " LIMIT "+os.str();
	}
	if( start ) {
			os.str("");
			os<<start;
			qr+= " OFFSET "+os.str();
	}

	return pqxx::Result(pqxx::NonTransaction(*pg).Exec(qr));
}

/**
 *
 */
void read_res_send( const pqxx::Result & res ) {
	cdaemonlog::size_type tcnt = res.size();
	if( ! tcnt ) {
			if( fdwrite(cso, &"Z", 1) != 1 ) throw wr_error();
			return;
	} else if( fdwrite(cso, &"F", 1) != 1
		|| fdwrite(cso, &tcnt, sizeof(tcnt)) != sizeof(tcnt)) 
			throw wr_error();
		
	cdaemonlog::size_type i, stmp;
	const char *tmp;
	for( i=0; i<tcnt; ++ i ) {
			tmp = res[i][0].c_str();
			if( ! tmp ) tmp = "";
			if( fdwrstr(cso, tmp) == -1 ) throw wr_error();

			tmp = res[i][2].c_str();
			if( ! tmp ) tmp = "0";
			pqxx::FromString<cdaemonlog::size_type>(tmp, stmp);
			if( fdwrite(cso, &stmp, sizeof(stmp)) == -1 ) throw wr_error(); 

			tmp = res[i][5].c_str();
			if( ! tmp ) tmp = "0";
			pqxx::FromString<cdaemonlog::size_type>(tmp, stmp);
			if( fdwrite(cso, &stmp, sizeof(stmp)) == -1 ) throw wr_error();

			tmp = res[i][4].c_str();
			if( ! tmp ) tmp = "";
			if( fdwrstr(cso, tmp) == -1 ) throw wr_error();

			tmp = res[i][3].c_str();
			if( ! tmp ) tmp = "";
			if( fdwrstr(cso, tmp) == -1 ) throw wr_error();
	
			tmp = res[i][1].c_str();
			if( ! tmp ) tmp = "";
			if( fdwrstr(cso, tmp) == -1 ) throw wr_error();
	}
}

/**
 * read informations from database 
 */
void cmd_log_read_dom_log() {
	cdaemonlog::size_type start, cnt;

	if( fdrdstr(cso, dom) != -1
		&& fdrdstr(cso, login) != -1
		&& fdread(cso, &start, sizeof(start)) == sizeof(start)
		&& fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {

			read_res_send(log_read_dom_log(dom, login, start, cnt));
	} else 
			throw rd_error();
}

/**
 * read informations from database 
 */
void cmd_dom_log_read_log() {
	cdaemonlog::size_type start, cnt;

	if( fdrdstr(cso, dom) != -1
		&& fdrdstr(cso, login) != -1
		&& fdread(cso, &start, sizeof(start)) == sizeof(start)
		&& fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {

			read_res_send(dom_log_read_log(dom, login, start, cnt));
	} else 
			throw rd_error();
}

/**
 *
 */
void cnt_res_send( const pqxx::Result & res ) {
	cdaemonlog::size_type cnt = res.size();
	if( cnt != 1 ) {
			if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
			return;
	} else if( fdwrite(cso, &"F", 1) != 1 )
			throw wr_error();
		
	const char *tmp;
	tmp = res[0][0].c_str();
	if( ! tmp ) tmp = "0";
	pqxx::FromString<cdaemonlog::size_type>(tmp, cnt);
	if( fdwrite(cso, &cnt, sizeof(cnt)) == -1 ) throw wr_error(); 
}

/**
 * read informations from database 
 */
void cmd_log_cnt_dom_log() {
	if( fdrdstr(cso, dom) != -1
		&& fdrdstr(cso, login) != -1 ) {

			cnt_res_send(pqxx::NonTransaction(*pg).Exec(
				"SELECT COUNT(*) FROM log WHERE domain="
				+pqxx::Quote(dom,false)+" AND login="+pqxx::Quote(login,false)));
	} else 
			throw rd_error();
}

/**
 * read informations from database 
 */
void cmd_dom_log_cnt_log() {
	if( fdrdstr(cso, dom) != -1
		&& fdrdstr(cso, login) != -1 ) {
			str2tb(dom);

			cnt_res_send(pqxx::NonTransaction(*pg).Exec(
				"SELECT COUNT(*) FROM \""+dom+"_log\" WHERE login="
				+pqxx::Quote(login,false)));
	} else 
			throw rd_error();
}

/**
*/
void (*cmd_proc( char cmd )) () {
	switch(cmd) {
	case cdaemonlog::cmd_write:
			return &cmd_write;
	case cdaemonlog::cmd_log_read_dom_log:
			return &cmd_log_read_dom_log;
	case cdaemonlog::cmd_dom_log_read_log:
			return &cmd_dom_log_read_log;
	case cdaemonlog::cmd_log_cnt_dom_log:
			return &cmd_log_cnt_dom_log;
	case cdaemonlog::cmd_dom_log_cnt_log:
			return &cmd_dom_log_cnt_log;
	default:
		return NULL;
	}
}
/**
*/
char child(int fd)
{
	char cmd;
	void (*run)();

	cso = fd;

	switch( fdread(cso, &cmd, 1) ) {
	case 0: return 1;
	case -1: 
			cerr<<"child: read error: "<<strerror(errno)<<endl;
			return 2;
	}
	// process cmd
	if( ! (run = cmd_proc(cmd)) ) {
			cerr << "child: unknown command: " << (int) cmd << endl;
			return 2;
	}

	try {
			(*run)();
			return 0;
	} catch( const rd_error & e ) {
            cerr<<"child: exception: "<<e.what()<<endl;
    } catch( const wr_error & e ) {
            cerr<<"child: exception: "<<e.what()<<endl;
    } catch( const exception & e ) {
            cerr<<"child: exception: "<<e.what()<<endl;
            if( fdwrite(cso, &"E", 1) != 1 ) cerr<<"child: write error: "<<strerror(errno)<<endl;
	}

	return 2;
}
