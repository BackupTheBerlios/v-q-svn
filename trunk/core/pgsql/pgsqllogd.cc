/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new-baz.com)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
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
#include "cdaemonauth.h"
#include "cdaemonchild.h"
#include "cdaemonmaster.h"

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

using namespace std;
using namespace vq;

class cpgsqllogd : public cdaemonchild {
		public:
				cpgsqllogd();
				virtual ~cpgsqllogd();

				void setup();
				int child( int );

				static cdaemonchild * alloc() { return new cpgsqllogd(); }

		protected:
				/// socket
				int cso;

				/// pgsql
				pqxx::Connection *pg;

				// child
				string msg, dom, login, ip;
				cdaemonlog::result res;
				cdaemonlog::service ser;
				string sser, sres;

				void write( const string &log, const string &ser, 
						const string &res, const string &ip, 
						const string &msg );

				void write( const string & dom, const string &log,
						const string &ser, const string &res,
						const string &ip, const string &msg );
				
				void write_dom( const string & dom, const string &log,
		                const string &ser, const string &res,
			            const string &ip, const string &msg );

				void cmd_write();

				pqxx::Result dom_log_read_log(const string &dom, 
						const string &log, cdaemonlog::size_type start, 
						cdaemonlog::size_type cnt );

				pqxx::Result log_read_dom_log(const string &dom, 
						const string &log, cdaemonlog::size_type start, 
						cdaemonlog::size_type cnt );

				void read_res_send( const pqxx::Result & res );

				void cmd_log_read_dom_log();

				void cmd_dom_log_read_log();

				void cnt_res_send( const pqxx::Result & res );

				void cmd_log_cnt_dom_log();

				void cmd_dom_log_cnt_log();

				void (cpgsqllogd::* cmd_proc( char cmd )) ();

				string int2str( int i );
};

/**
 *
 */
cpgsqllogd::cpgsqllogd() : pg(0) {
}

/**
 *
 */
cpgsqllogd::~cpgsqllogd() {
	delete pg;
}

/**
 * \exception runtime_error if it can't create socket
 */
void cpgsqllogd::setup() {
	if( !(pg = new pqxx::Connection(ac_pgsql.val_str().c_str())) ) 
			throw runtime_error((string)"setup: no memory"); 
}

/**
 *
 */
string cpgsqllogd::int2str( int i ) {
	ostringstream os;
	os<<i;
	return ! os ? "0" : os.str();
}

/**
 *
 */
void cpgsqllogd::write( const string &log, 
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
void cpgsqllogd::write( const string & dom, const string &log, 
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
void cpgsqllogd::write_dom( const string & dom, const string &log, 
		const string &ser, const string &res, 
		const string &ip, const string &msg ) {

	pqxx::Result x(pqxx::NonTransaction(*pg).Exec(
		(string)"INSERT INTO \""+str2tb(dom)+"_log\" (LOGIN,SERVICE,RESULT,IP,MSG) "
		"VALUES("+pqxx::Quote(log, false)+","+pqxx::Quote(ser, false)+","
		+pqxx::Quote(res, false)+","+pqxx::Quote(ip, false)+","+pqxx::Quote(msg, false)+")"));
}	

/**
 * write information in database 
 */
void cpgsqllogd::cmd_write() {
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
pqxx::Result cpgsqllogd::dom_log_read_log(const string &dom, const string &log, 
		cdaemonlog::size_type start, cdaemonlog::size_type cnt ) {
	string qr("SELECT id,time,service,msg,ip,result FROM \""+str2tb(dom)+"_log\""
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
pqxx::Result cpgsqllogd::log_read_dom_log(const string &dom, const string &log, 
		cdaemonlog::size_type start, cdaemonlog::size_type cnt ) {
	string tb(str2tb(dom));

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
void cpgsqllogd::read_res_send( const pqxx::Result & res ) {
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
void cpgsqllogd::cmd_log_read_dom_log() {
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
void cpgsqllogd::cmd_dom_log_read_log() {
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
void cpgsqllogd::cnt_res_send( const pqxx::Result & res ) {
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
void cpgsqllogd::cmd_log_cnt_dom_log() {
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
void cpgsqllogd::cmd_dom_log_cnt_log() {
	if( fdrdstr(cso, dom) != -1
		&& fdrdstr(cso, login) != -1 ) {
			cnt_res_send(pqxx::NonTransaction(*pg).Exec(
				"SELECT COUNT(*) FROM \""+str2tb(dom)+"_log\" WHERE login="
				+pqxx::Quote(login,false)));
	} else 
			throw rd_error();
}

/**
*/
void (cpgsqllogd::* cpgsqllogd::cmd_proc( char cmd )) () {
	switch(cmd) {
	case cdaemonlog::cmd_write:
			return &cpgsqllogd::cmd_write;
	case cdaemonlog::cmd_log_read_dom_log:
			return &cpgsqllogd::cmd_log_read_dom_log;
	case cdaemonlog::cmd_dom_log_read_log:
			return &cpgsqllogd::cmd_dom_log_read_log;
	case cdaemonlog::cmd_log_cnt_dom_log:
			return &cpgsqllogd::cmd_log_cnt_dom_log;
	case cdaemonlog::cmd_dom_log_cnt_log:
			return &cpgsqllogd::cmd_dom_log_cnt_log;
	default:
		return NULL;
	}
}
/**
*/
int cpgsqllogd::child(int fd) {
	try {
			char cmd;
			void (cpgsqllogd::* run)();

			cso = fd;

			switch( fdread(cso, &cmd, 1) ) {
			case 0: return 1;
			case -1: 
					throw rd_error();
			}
			
			// process cmd
			if( ! (run = cmd_proc(cmd)) ) {
					ostringstream os;
					os<<cmd;
					throw bad_command(os.str());
			}

			((*this).*run)();
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

/**
 * Plugin structure
 */
cdaemonmaster::daemon_info pgsqllogd_info = { & cpgsqllogd::alloc };
