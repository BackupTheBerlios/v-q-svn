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
	cpgsqllog::cpgsqllog( const char * pginfo ) std_try {
		clear();
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

	/**
	 *
	 */
	void cpgsqllog::clear() std_try {
		this->ip.clear();
		this->ser = pqxx::ToString(service_type());
		this->dom.clear();
		this->log.clear();
	} std_catch
	/**
	 *
	 */
	void cpgsqllog::ip_set( const char* ip ) std_try {
		if( ! ip )
				throw ::vq::null_error(__FILE__, __LINE__);
		this->ip = text::lower(ip);
	} std_catch
	
	/**
	 *
	 */
	void cpgsqllog::service_set( service_type ser ) std_try {
		this->ser = pqxx::ToString(ser);
	} std_catch

	/**
	 *
	 */
    void cpgsqllog::domain_set( const char* dom ) std_try {
		if( ! dom )
				throw ::vq::null_error(__FILE__, __LINE__);
		this->dom = text::lower(dom);
	} std_catch

	/**
	 *
	 */
    void cpgsqllog::login_set( const char* log ) std_try {
		if( ! log )
				throw ::vq::null_error(__FILE__, __LINE__);
		this->log = text::lower(log);
	} std_catch
 
	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::write( result_type res, const char * msg ) std_try {
		if( ! msg )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		pqxx::NonTransaction(*pg).Exec(
			"SELECT log_write("
			+pqxx::Quote(this->ip)+","
			+this->ser+","
			+pqxx::Quote(this->dom)+","
			+pqxx::Quote(this->log)+","
			+pqxx::ToString(static_cast<unsigned>(res))+","
			+pqxx::Quote(msg)+')');
		
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::count( size_type & cnt ) std_try {
		return count_by_func( "log_count()" , cnt);
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::count_by_dom( size_type & cnt ) std_try {
  		return count_by_func( "log_count_by_dom("+pqxx::Quote(this->dom)+")", 
			cnt);
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::count_by_user( size_type & cnt ) std_try {
  		return count_by_func( "log_count_by_user("+pqxx::Quote(this->dom)
			+','+pqxx::Quote(this->log)+")", cnt);
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::count_by_func( const std::string & func,
			size_type & cnt ) std_try {

		string qr("SELECT "+func);
		Result res(pqxx::NonTransaction(*pg).Exec(qr));
		if( res.empty() ) return lr(::vq::ivq::err_func_res, func.c_str());
		
		cnt = res[0][0].as< size_type >(0);
  		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::read( size_type start, size_type end,
			log_entry_list_out les ) std_try {
		return read_by_func(make_pair(0, "log_read()"), start, end, les);
	} std_catch
	
	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::read_by_dom( size_type start, size_type end,
			log_entry_list_out les ) std_try {
		return read_by_func(
			make_pair(rbf_ignore_domain, "log_read_by_dom("+pqxx::Quote(this->dom)+")"), 
			start, end, les);
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::read_by_user( size_type start, size_type end,
			log_entry_list_out les ) std_try {
		return read_by_func(
			make_pair(rbf_ignore_domain | rbf_ignore_login,
				"log_read_by_user("+pqxx::Quote(this->dom)
				+','+pqxx::Quote(this->log)+")" ), 
			start, end, les);
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::read_by_func(const rbf_func_desc_type &func, 
			size_type start, size_type cnt, log_entry_list_out les ) std_try {

		string qr( "SELECT id_log,time,ip,service,result,msg" );
		if( ! (func.first & rbf_ignore_domain) ) qr += ",domain"; 
		if( ! (func.first & rbf_ignore_login) ) qr += ",login"; 
		qr += " FROM "+func.second+" ORDER BY time DESC";
		
		if( cnt ) {
				qr += " LIMIT "+ToString(cnt);
		}
		if( start ) {
				qr+= " OFFSET "+ToString(start);
		}
	
		Result res(pqxx::NonTransaction(*pg).Exec(qr));
		Result::size_type s = res.size();

		les = new log_entry_list(static_cast<CORBA::ULong>(s));
		les->length(s);
		for( Result::size_type i=0, idx=0; i<s; ++i, idx=0 ) {
				les[i].id_log = CORBA::string_dup(
					res[i][idx].is_null() ? "" : res[i][idx].c_str() );
				++idx;
				les[i].time = CORBA::string_dup(
					res[i][idx].is_null() ? "" : res[i][idx].c_str() );
				++idx;
				les[i].ip = CORBA::string_dup(
					res[i][idx].is_null() ? "" : res[i][idx].c_str() );
				++idx;
				les[i].ser = res[i][idx].as< ::vq::ilogger::service_type >
						(::vq::ilogger::ser_unknown);
				++idx;
				les[i].res = res[i][idx].as< ::vq::ilogger::result_type >
						(::vq::ilogger::re_unknown);
				++idx;
				les[i].msg = CORBA::string_dup(
					res[i][idx].is_null() ? "" : res[i][idx].c_str() );
				++idx;
				if( ! (func.first & rbf_ignore_domain) ) { 
						les[i].domain = CORBA::string_dup(
							res[i][idx].is_null() ? "" : res[i][idx].c_str() );
						++idx;
				}
				if( ! (func.first & rbf_ignore_login) ) {
						les[i].login = CORBA::string_dup(
							res[i][idx].is_null() ? "" : res[i][idx].c_str() );
						++idx;
				}
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::rm_all() std_try {
		return rm_by_func( "log_rm_all()");
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::rm_by_dom() std_try {
  		return rm_by_func( "log_rm_by_dom("+pqxx::Quote(this->dom)+")" );
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::rm_by_user() std_try {
  		return rm_by_func( "log_rm_by_user("+pqxx::Quote(this->dom)
			+','+pqxx::Quote(this->log)+")" );
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::rm_by_func( const std::string & func ) std_try {
		pqxx::NonTransaction(*pg).Exec("SELECT "+func);
  		return lr(::vq::ivq::err_no, "");
	} std_catch

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
