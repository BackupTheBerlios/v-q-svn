/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2005 Pawel Niewiadomski (new@foo-baz.com)

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
#include <iostream>

namespace POA_vq {

	using namespace std;
	using namespace vq;
	using namespace pqxx;
	
	/**
	 *
	 */
	cpgsqllog::cpgsqllog( const service_conf & c ) 
			: pool(c.pg_info, c.pgs_pool), conf(c) std_try {
		clear();
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
		this->ser = pqxx::to_string(service_type());
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
		this->ser = pqxx::to_string(ser);
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
	cpgsqllog::error * cpgsqllog::write( result_type result, const char * msg ) std_try {
		if( ! msg )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		cpgsqlpool::value_ptr pg(pool.get());
		pqxx::result res(pqxx::nontransaction(*pg.get()).exec(
			"SELECT log_write('"+sqlesc(this->ip)+"',"+this->ser+",'"
			+sqlesc(this->dom)+"','"+sqlesc(this->log)+"',"
			+to_string(static_cast<unsigned>(result))+",'"
			+sqlesc(msg)+"')"));
		
		if( res.empty() || res[0][0].is_null() 
			|| strcmp(res[0][0].c_str(), "0") )
				return lr(::vq::ivq::err_func_res, "LOG_WRITE");

		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::count( size_type & cnt ) std_try {
		return count_by_func( "vq_view_log_count" , cnt);
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::count_by_dom( size_type & cnt ) std_try {
  		return count_by_func( 
			(std::string)"vq_view_log_count_by_dom WHERE domain='"
			+sqlesc(this->dom)+'\'', cnt);
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::count_by_user( size_type & cnt ) std_try {
  		return count_by_func( 
			(std::string)"vq_view_log_count_by_user WHERE domain='"
			+sqlesc(this->dom)+"' AND login='"+sqlesc(this->log)+'\'', cnt);
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::count_by_func( const std::string & func,
			size_type & cnt ) std_try {

		string qr("SELECT count FROM ");
		qr += func;
		cpgsqlpool::value_ptr pg(pool.get());
		result res(pqxx::nontransaction(*pg.get()).exec(qr));
		
		cnt = res.empty() ? 0 : res[0][0].as< size_type >(0);
  		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::read( size_type start, size_type end,
			log_entry_list_out les ) std_try {
		return read_by_func(make_pair(0, "vq_view_log_read"), start, end, les);
	} std_catch
	
	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::read_by_dom( size_type start, size_type end,
			log_entry_list_out les ) std_try {
		return read_by_func(
			make_pair(rbf_ignore_domain, 
				(std::string)"vq_view_log_read WHERE domain='"+sqlesc(this->dom)+'\''), 
			start, end, les);
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::read_by_user( size_type start, size_type end,
			log_entry_list_out les ) std_try {
		return read_by_func(
			make_pair(rbf_ignore_domain | rbf_ignore_login,
				(std::string)"vq_view_log_read WHERE domain='"+sqlesc(this->dom)
				+"' AND login='"+sqlesc(this->log)+'\'' ), 
			start, end, les);
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::read_by_func(const rbf_func_desc_type &func, 
			size_type start, size_type cnt, log_entry_list_out les ) std_try {
		
		les = new log_entry_list;

		string qr( "SELECT id_log,time,ip,service,result,msg" );
		if( ! (func.first & rbf_ignore_domain) ) qr += ",domain"; 
		if( ! (func.first & rbf_ignore_login) ) qr += ",login"; 
		qr += " FROM "+func.second+" ORDER BY time DESC";
		
		if( cnt ) {
				qr += " LIMIT "+to_string(cnt);
		}
		if( start ) {
				qr+= " OFFSET "+to_string(start);
		}

		cpgsqlpool::value_ptr pg(pool.get());
		result res(pqxx::nontransaction(*pg.get()).exec(qr));
		result::size_type s = res.size();

		les->length(s);
		for( result::tuple::size_type i=0, idx=0; i<s; ++i, idx=0 ) {
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
	cpgsqllog::error * cpgsqllog::read_by_sql(const char * sql, 
			size_type start, size_type cnt, string_list2_out les ) std_try {
		if( ! sql )
			throw ::vq::null_error(__FILE__, __LINE__);

		les = new string_list2;

		if( ! conf.read_by_sql ) return lr(::vq::ivq::err_perm, "read_by_sql");

		string qr( sql );
		
		if( cnt ) {
				qr += " LIMIT "+to_string(cnt);
		}
		if( start ) {
				qr+= " OFFSET "+to_string(start);
		}

		cpgsqlpool::value_ptr pg(pool.get());
		pqxx::work trans(*pg.get());
		trans.exec("SET TRANSACTION READ ONLY"); // for security

		result res(trans.exec(qr));
		result::size_type s = res.size(), fs;

		les->length(s);
		for( result::tuple::size_type i=0; i<s; ++i ) {
				fs = res[i].size();
				les[i].length(fs);
				for( result::tuple::size_type j=0; j < fs; ++j ) {
						les[i][j] = CORBA::string_dup( res[i][j].is_null() ? "" : res[i][j].c_str() );
				}
		}
		trans.abort(); // for security
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
  		return rm_by_func( "log_rm_by_dom("+'\''+sqlesc(this->dom)+'\''+")" );
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::rm_by_user() std_try {
  		return rm_by_func( "log_rm_by_user("+'\''+sqlesc(this->dom)+'\''
			+','+'\''+sqlesc(this->log)+'\''+")" );
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::rm_by_sql( const char * where ) std_try {
		if( ! where )
			throw ::vq::null_error(__FILE__, __LINE__);

		if( ! conf.rm_by_sql ) return lr(::vq::ivq::err_perm, "rm_by_sql");

  		return rm_by_func( "log_rm_by_sql("+'\''+sqlesc(where)+'\''+")" );
	} std_catch

	/**
	 *
	 */
	cpgsqllog::error * cpgsqllog::rm_by_func( const std::string & func ) std_try {
		cpgsqlpool::value_ptr pg(pool.get());
		pqxx::result res(pqxx::nontransaction(*pg.get()).exec("SELECT "+func));

		if( res.empty() || res[0][0].is_null() 
			|| strcmp(res[0][0].c_str(), "0") )
				return lr(::vq::ivq::err_func_res, func);

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
