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
#include "cpgsqllog.hpp"

#include <text.hpp>

#include <stdexcept>

namespace POA_vq {

	using namespace std;
	using namespace vq;
	using namespace pqxx;
	
	/**
	 *
	 */
	cpgsqllog::cpgsqllog( const char * pginfo ) 
			: ser(static_cast<cpgsqllog::service>(0)) std_try {
		pg.reset(new Connection(pginfo));
		if( ! pg.get() || ! pg->is_open() ) {
				throw ::vq::except(
						static_cast<const char*>("can't create connection"),
						__FILE__, __LINE__ ); 
		}
	} std_catch
	
	/**
	 *
	 */
	cpgsqllog::~cpgsqllog() std_try {
	} std_catch

	void cpgsqllog::ip_set( const char* ip ) std_try {
		if( ! ip )
				throw ::vq::null_error(__FILE__, __LINE__);
		this->ip = ip;
	} std_catch
	
	void cpgsqllog::service_set( service ser ) std_try {
		this->ser = ser;
	} std_catch

    void cpgsqllog::domain_set( const char* dom ) std_try {
		if( ! dom )
				throw ::vq::null_error(__FILE__, __LINE__);
		this->dom = dom;
	} std_catch

    void cpgsqllog::login_set( const char* log ) std_try {
		if( ! log )
				throw ::vq::null_error(__FILE__, __LINE__);
		this->log = log;
	} std_catch
 
	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::write( result res, const char * msg ) std_try {
		if( ! msg )
				throw ::vq::null_error(__FILE__, __LINE__);
		/*
	
		pqxx::Result x(pqxx::NonTransaction(*pg).Exec(
			"SELECT INSERT INTO log (LOGIN,SERVICE,RESULT,IP,MSG)"
			" VALUES("+pqxx::Quote(log, false)+","
			+pqxx::Quote(ser, false)+","+pqxx::Quote(res, false)+","
			+pqxx::Quote(ip, false)+","+pqxx::Quote(msg, false)+")"));
		*/
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::count( size_type & cnt ) std_try {
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::read( size_type start, size_type end,
			log_entry_list_out les ) std_try {
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
#if 0	
	/**
	 *
	 */
	pqxx::Result cpgsqllog::dom_log_read_log(const string &dom, const string &log, 
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
	} std_catch
	
	/**
	 *
	 */
	pqxx::Result cpgsqllog::log_read_dom_log(const string &dom, const string &log, 
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
	} std_catch
	
	/**
	 * read informations from database 
	 */
	void cpgsqllog::cmd_log_read_dom_log() {
		cdaemonlog::size_type start, cnt;
	
		if( fdrdstr(cso, dom) != -1
			&& fdrdstr(cso, login) != -1
			&& fdread(cso, &start, sizeof(start)) == sizeof(start)
			&& fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
	
				read_res_send(log_read_dom_log(dom, login, start, cnt));
		} else 
				throw rd_error();
	} std_catch
	
	/**
	 * read informations from database 
	 */
	void cpgsqllog::cmd_dom_log_read_log() {
		cdaemonlog::size_type start, cnt;
	
		if( fdrdstr(cso, dom) != -1
			&& fdrdstr(cso, login) != -1
			&& fdread(cso, &start, sizeof(start)) == sizeof(start)
			&& fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
	
				read_res_send(dom_log_read_log(dom, login, start, cnt));
		} else 
				throw rd_error();
	} std_catch
	
	/**
	 *
	 */
	void cpgsqllog::cnt_res_send( const pqxx::Result & res ) {
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
	} std_catch
	
	/**
	 * read informations from database 
	 */
	void cpgsqllog::cmd_log_cnt_dom_log() {
		if( fdrdstr(cso, dom) != -1
			&& fdrdstr(cso, login) != -1 ) {
	
				cnt_res_send(pqxx::NonTransaction(*pg).Exec(
					"SELECT COUNT(*) FROM log WHERE domain="
					+pqxx::Quote(dom,false)+" AND login="+pqxx::Quote(login,false)));
		} else 
				throw rd_error();
	} std_catch
	
	/**
	 * read informations from database 
	 */
	void cpgsqllog::cmd_dom_log_cnt_log() {
		if( fdrdstr(cso, dom) != -1
			&& fdrdstr(cso, login) != -1 ) {
				cnt_res_send(pqxx::NonTransaction(*pg).Exec(
					"SELECT COUNT(*) FROM \""+str2tb(dom)+"_log\" WHERE login="
					+pqxx::Quote(login,false)));
		} else 
				throw rd_error();
	} std_catch
#endif // #if 0

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::lr_wrap(err_code ec, const char *what,
			const char * file, CORBA::ULong line ) std_try {
		auto_ptr<error> err(new error);
		err->ec = ec;
		err->what = CORBA::string_dup(what); // string_dup not really needed
		err->file = CORBA::string_dup(file);
		err->line = line;
		return err.release();
	} std_catch

} // namespace POA_vq
