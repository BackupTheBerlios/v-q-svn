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
package com.foo_baz.v_q;
import com.foo_baz.v_q.ivqPackage.*;
import com.foo_baz.v_q.iloggerPackage.*;

import java.sql.*;
import javax.sql.*;
import java.util.*;
import org.omg.CORBA.*;

public class JDBCLog extends iloggerPOA {	
	/**
	 *
	 */
	public JDBCLog( Connection con ) throws SQLException {
		clear();
		this.con = con;
		this.con.setAutoCommit(true);
	}
	
	/**
	 *
	 */
	public void clear() {
		ip = new String();
		ser = ilogger.ser_unknown;
		dom = new String();
		log = new String();
	}
	/**
	 *
	 */
	public void ip_set( String ip ) {
		this.ip = ip.toLowerCase();
	}
	
	/**
	 *
	 */
	public void service_set( short ser ) {
		this.ser = ser;
	}

	/**
	 *
	 */
    public void domain_set( String dom ) {
		this.dom = dom.toLowerCase();
	}

	/**
	 *
	 */
    public void login_set( String log ) {
		this.log = log.toLowerCase();
	}
 
	/**
	 *
	 */
	public error write( short res, String msg ) throws null_error, db_error, except { try {
		CallableStatement call = con.prepareCall( "{ call log_write(?, ?, ?, ?, ?) }" );
		int idx=0;
		call.setString(idx++, ip);
		call.setShort(idx++, ser);
		call.setString(idx++, dom);
		call.setString(idx++, log);
		call.setShort(idx++, res);
		call.setString(idx++, msg);
		call.execute();
		
		return lr(err_code.err_no, "");
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( NullPointerException e ) {
		throw new null_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }

	/**
	 *
	 */
	public error count( IntHolder cnt ) throws null_error, db_error, except { try {
		cnt = new IntHolder(0);

		Statement st = null;
		ResultSet res = null; 
		try {
			st = con.createStatement();
			res = st.executeQuery("SELECT count FROM vq_view_log_count");
			while(res.next()) {
				cnt = new IntHolder(res.getInt(1));
			}
		} finally {
			try { if( res != null ) res.close(); } catch(Exception e) {}
			try { if( st != null ) st.close(); } catch(Exception e) {}
		}

		return lr(err_code.err_no, "");
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( NullPointerException e ) {
		throw new null_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }


	/**
	 *
	 */
	public error count_by_dom( IntHolder cnt ) throws null_error, db_error, except { try {
		cnt = new IntHolder(0);

		PreparedStatement st = null;
		ResultSet res = null;
		try {
			st = con.prepareStatement("SELECT count FROM vq_view_log_count_by_dom WHERE domain=?");
			st.setString(1, dom);
			res = st.executeQuery();
			while(res.next()) {
				cnt = new IntHolder(res.getInt(1));
			}
		} finally {
			try { if( res != null ) res.close(); } catch(Exception e) {}
			try { if( st != null) st.close(); } catch(Exception e) {}
		}

		return lr(err_code.err_no, "");
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( NullPointerException e ) {
		throw new null_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }

	/**
	 *
	 */
	public error count_by_user( IntHolder cnt ) throws null_error, db_error, except { try {
		cnt = new IntHolder(0);

		PreparedStatement st = null;
		ResultSet res = null;
		try {
			st = con.prepareStatement(
				"SELECT count FROM vq_view_log_count_by_dom WHERE domain=? AND login=?");
			st.setString(1, dom);
			st.setString(2, log);
			res = st.executeQuery();
			while(res.next()) {
				cnt = new IntHolder(res.getInt(1));
			}
		} finally {
			try { if( res != null ) res.close(); } catch(Exception e) {}
			try { if( st != null) st.close(); } catch(Exception e) {}
		}

		return lr(err_code.err_no, "");
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( NullPointerException e ) {
		throw new null_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }

	/**
	 *
	 */
	public error read( int start, int end, log_entry_listHolder les ) 
			throws null_error, db_error, except {
		return read_by_func( rbf_all, start, end, les);
	}
	
	/**
	 *
	 */
	public error read_by_dom( int start, int end, log_entry_listHolder les ) 
			throws null_error, db_error, except {
		return read_by_func( rbf_by_dom, start, end, les);
	}

	/**
	 *
	 */
	public error read_by_user( int start, int end, log_entry_listHolder les ) 
			throws null_error, db_error, except {
		return read_by_func( rbf_by_user, start, end, les );
	}

	/**
	 *
	 */
	protected error read_by_func( short rbf, int start, int cnt, log_entry_listHolder les ) 
			throws null_error, db_error, except { try {
		les = new log_entry_listHolder();
		ArrayList ales = new ArrayList();

		PreparedStatement st = null;
		ResultSet res = null;

		try {
			String qr = null;
			if( rbf == rbf_all )
				qr = "SELECT id_log,time,ip,service,result,msg,domain,login "
					+ "FROM vq_view_log_read ORDER BY time DESC";
			if( rbf == rbf_by_dom )
				qr = "SELECT id_log,time,ip,service,result,msg,login "
					+ "FROM vq_view_log_read WHERE domain=? ORDER BY time DESC";
			if( rbf == rbf_by_user )
				qr = "SELECT id_log,time,ip,service,result,msg FROM vq_view_log_read WHERE "
					+" domain=? AND login=? ORDER BY time DESC";


			st = con.prepareStatement(qr);
			if( rbf == rbf_by_dom || rbf == rbf_by_user )
				st.setString(1, dom);
			if( rbf == rbf_by_user )
				st.setString(2, log);

			res = st.executeQuery();

			if( ! res.absolute(start+1) ) return lr(err_code.err_noent, "");
			--cnt; // if it's zero means that we want to read all entries
			for( int idx=0; res.next() && cnt-- != 0; idx = 0 ) {
				log_entry le = new log_entry();
				
				le.id_log = res.getString(idx);
				if( res.wasNull() ) le.id_log = "";
				++idx;
				le.time = res.getTime(idx).toString();
				if( res.wasNull() ) le.time = "";
				++idx;
				le.ip = res.getString(idx);
				if( res.wasNull() ) le.ip = "";
				++idx;
				le.ser = res.getShort(idx);
				if( res.wasNull() ) le.ser = ilogger.ser_unknown;
				++idx;
				le.res = res.getShort(idx);
				if( res.wasNull() ) le.res = ilogger.re_unknown;
				++idx;
				le.msg = res.getString(idx);
				if( res.wasNull() ) le.msg = "";
				++idx;

				if( rbf != rbf_by_user && rbf != rbf_by_dom ) { 
					le.domain = res.getString(idx);
					if( res.wasNull() ) le.domain = "";
					++idx;
				}
				if( rbf != rbf_by_user ) {
					le.login = res.getString(idx);
					if( res.wasNull() ) le.login = "";
					++idx;
				}
				ales.add(le);
			}

			les.value = (log_entry[])ales.toArray();
		} finally {
			try { if( res != null ) res.close(); } catch(Exception e) {}
			try { if( st != null) st.close(); } catch(Exception e) {}
		}

		return lr(err_code.err_no, "");
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( NullPointerException e ) {
		throw new null_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }

	/**
	 *
	 */
	public error rm_all() throws null_error, db_error, except { try {
		CallableStatement call = con.prepareCall( "{ call log_rm_all() }" );
		call.execute();
		
		return lr(err_code.err_no, "");
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( NullPointerException e ) {
		throw new null_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }

	/**
	 *
	 */
	public error rm_by_dom() throws null_error, db_error, except { try {
		CallableStatement call = con.prepareCall( "{ call log_rm_by_dom(?) }" );
		int idx=0;
		call.setString(idx++, dom);
		call.execute();
		
		return lr(err_code.err_no, "");
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( NullPointerException e ) {
		throw new null_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }

	/**
	 *
	 */
	public error rm_by_user() throws null_error, db_error, except { try {
		CallableStatement call = con.prepareCall( "{ call log_rm_by_dom(?, ?) }" );
		int idx=0;
		call.setString(idx++, dom);
		call.setString(idx++, log);
		call.execute();
		
		return lr(err_code.err_no, "");
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( NullPointerException e ) {
		throw new null_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }

	/**
	 *
	 */
	protected error lr( err_code ec, String what ) {
		error err = new error();
		err.ec = ec;
		err.what = what; // string_dup not really needed
		err.file = this.toString();
		err.line = 0;
		return err;
	}

	protected String ip;
	protected String log;
	protected String dom;
	protected short ser;
	protected Connection con;

	protected static final short rbf_all = 1;
	protected static final short rbf_by_dom = 2;
	protected static final short rbf_by_user = 3;
} // namespace POA_vq
