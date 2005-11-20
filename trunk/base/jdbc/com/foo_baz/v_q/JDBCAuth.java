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
import com.foo_baz.v_q.iauthPackage.*;

import java.sql.*;
import javax.sql.*;
import java.util.*;
import java.net.*;
import org.omg.CORBA.*;

public class JDBCAuth extends iauthPOA {

	/**
	 *
	 */
	public JDBCAuth( Connection con ) throws SQLException {
		this.con = con;
		this.con.setAutoCommit(true);
	}

	/**
	*/
	public error dom_add( String dom, IntHolder dom_id )
			throws null_error, db_error, except { try {
		dom_id.value = 0;

		dom = dom.toLowerCase();

		CallableStatement call = con.prepareCall("{? = call dom_add(?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setString(idx++, dom);
		call.execute();

		dom_id.value = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {}

		if( wasNull )
			return lr(err_code.err_func_res, "DOM_ADD");

		if( dom_id.value < 0 ) {
			return dom_id.value == -1 ? lr(err_code.err_exists, dom)
				: lr(err_code.err_func_res, "DOM_ADD");
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
	*/
	public error dom_id( String dom, IntHolder dom_id )
			throws null_error, db_error, except { try {
		dom_id.value = 0;
		dom = dom.toLowerCase();

		String func = "DOM_ID";
		CallableStatement call = con.prepareCall( "{ ? = call "+func+"(?) }" );
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setString(idx++, dom);
		call.execute();

		dom_id.value = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {};

		if( wasNull || dom_id.value == -1 ) 
			return lr(err_code.err_noent, dom);
		return lr(err_code.err_no, "");
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( NullPointerException e ) {
		throw new null_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }

	/**
	*/
	public error dom_name( int dom_id, StringHolder domain )
			throws null_error, db_error, except { try {
		domain.value = "";

		PreparedStatement st = null;
		ResultSet res = null;
		err_code ec = err_code.err_noent;
		String ec_msg = Integer.toString(dom_id);

		try {
			st = con.prepareStatement( 
				"SELECT domain FROM vq_view_DOM_NAME WHERE id_domain=?" );
			st.setInt(1, dom_id);
			res = st.executeQuery();
			while(res.next()) {
				domain.value = res.getString(1);
				if( ! res.wasNull() ) {
					ec = err_code.err_no;
					ec_msg = "";
					break;
				}
			}
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
		}
		return lr(ec, ec_msg);
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( NullPointerException e ) {
		throw new null_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }

	/**
	*/
	public error dom_rm( int dom_id ) 
			throws null_error, db_error, except {
		return func_rm( dom_id, "DOM_RM" );
	}

	/**
	 *
	 */
	public error dom_ls( domain_info_listHolder dis )
			throws null_error, db_error, except { try {
		dis.value = new domain_info[0];
		Statement st = null;
		ResultSet res = null;
		ArrayList adis = new ArrayList();

		try {
			st = con.createStatement();
			res = st.executeQuery("SELECT id_domain,domain FROM vq_view_dom_ls ORDER BY domain");

			for( int idx=1; res.next(); idx=1) {
				domain_info di = new domain_info();
				di.id_domain = res.getInt(idx++);
				if( res.wasNull() ) di.id_domain = 0;
				di.domain = res.getString(idx++);
				if( res.wasNull() ) di.domain = "";
				adis.add(di);
			}

			dis.value = new domain_info [adis.size()];
			for(int i=0, s=adis.size(); i<s; ++i) 
				dis.value[i] = (domain_info) adis.get(i);
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
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
	*/
	public error user_add( user_info ai, boolean is_banned ) 
			throws null_error, db_error, except { try {
		CallableStatement call = con.prepareCall("{ ? = call user_add(?, ?, ?, ?, ?, ?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setInt(idx++, ai.id_domain);
		call.setString(idx++, ai.login.toLowerCase());
		call.setString(idx++, ai.pass);
		call.setString(idx++, ai.dir);
		call.setInt(idx++, ai.flags);
		call.setBoolean(idx++, is_banned);
		call.execute();
		
		int res = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {};
		
		if( wasNull ) {
			return lr(err_code.err_func_res, "USER_ADD");
		}

		if( res < 0 ) {
			switch( res ) {
			case -3:
				return lr(err_code.err_noent, Integer.toString(ai.id_domain));
			case -2:
				return lr(err_code.err_user_inv, "");
			case -1:
				return lr(err_code.err_exists, "");
			default:
				return lr(err_code.err_func_res, "USER_ADD");
			} 
		}

		if( 0 == res )
			return lr(err_code.err_no, "");
		
		return lr(err_code.err_func_res, "USER_ADD");
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
	public error da_add( int dom_id, String ali ) 
			throws null_error, db_error, except {
		return da_dip_add(dom_id, ali, "DA_ADD");
	}

	/**
	 *
	 */
	public error dip_add( int dom_id, String ali ) 
			throws null_error, db_error, except {

		InetAddress ia = null;
		try {
			ia = InetAddress.getByName(ali);
		} catch( Exception e ) {
			ia = null;
		}
		if( ia == null )
			return lr(err_code.err_dom_inv, ali);

		//return da_dip_add(dom_id, ia.getHostAddress(), "DIP_ADD");
		return da_dip_add(dom_id, ali, "DIP_ADD");
	}

	/**
	 *
	 */
	public error da_rm( String rea ) 
			throws null_error, db_error, except {
		return da_dip_rm( rea, "DA_RM" );
	}
	
	/**
	 *
	 */
	public error dip_rm( String rea )
			throws null_error, db_error, except {
		return da_dip_rm( rea, "DIP_RM" );
	}

	/**
	 *
	 */
	public error da_ls_by_dom( int dom_id, string_listHolder reas ) 
			throws null_error, db_error, except {
		return da_dip_ls_by_dom( dom_id, reas, "alias", "vq_view_DA_LS" );
	}

	/**
	 *
	 */
	public error dip_ls_by_dom( int dom_id, string_listHolder reas ) 
			throws null_error, db_error, except {
		return da_dip_ls_by_dom( dom_id, reas, "ip", "vq_view_DIP_LS" );
	}

	/**
	 *
	 */
	public error da_dip_ls_by_dom( int dom_id, string_listHolder reas, 
			String field, String view )
			throws null_error, db_error, except { try {

		reas.value = new String[0];
		PreparedStatement st = null;
		ResultSet res = null;
		ArrayList areas = new ArrayList();

		try {
			st = con.prepareStatement(
				"SELECT "+field+" FROM "+view+" WHERE id_domain=? ORDER BY "+field);
			st.setInt(1, dom_id);
			res = st.executeQuery();

			while( res.next() ) {
				String item = res.getString(1);
				if( res.wasNull() ) item = "";
				areas.add(item);
			}

			reas.value = new String [areas.size()];
			for( int i=0, s=areas.size(); i<s; ++i )
				reas.value[i] = (String) areas.get(i);
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
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
	public error user_pass( int dom_id, String login, String pass ) 
			throws null_error, db_error, except { try {
		
		CallableStatement call = con.prepareCall("{ ? = call user_pass(?, ?, ?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setInt(idx++, dom_id);
		call.setString(idx++, login.toLowerCase());
		call.setString(idx++, pass);
		call.execute();
		
		int res = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {}
		
		if( wasNull ) {
			return lr(err_code.err_func_res, "USER_PASS");
		}

		if( res < 0 ) {
			switch( res ) {
			case -1:
				return lr(err_code.err_noent, "");
			default:
				return lr(err_code.err_func_res, "USER_PASS");
			} 
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
	*/
	public error user_rm( int dom_id, String login ) 
			throws null_error, db_error, except { try {
		String func = "USER_RM";
		CallableStatement call = con.prepareCall("{? = call "+func+"(?, ?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setInt(idx++, dom_id);
		call.setString(idx++, login);
		call.execute();

		int res = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {}
		
		if( wasNull ) {
			return lr(err_code.err_func_res, func);
		}

		if( 0 == res )
			return lr(err_code.err_no, "");
			
		return lr(err_code.err_func_res, func);
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( NullPointerException e ) {
		throw new null_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }

	/**
	*/
	public error user_ex( int dom_id, String login )
			throws null_error, db_error, except { try {
		PreparedStatement st = null;
		ResultSet res = null;
		err_code ec = err_code.err_noent;
		String ec_msg = "";

		try {
			st = con.prepareStatement( 
				"SELECT COUNT(*) FROM vq_view_USER_EX "
				+ "WHERE id_domain=? AND login=?" );
			int idx=1;
			st.setInt(idx++, dom_id);
			st.setString(idx++, login.toLowerCase());
			res = st.executeQuery();
			while(res.next()) {
				ec = res.getInt(1) != 0
					? err_code.err_no : err_code.err_noent;
				ec_msg = "";
				break;
			}
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
		}
		return lr(ec, ec_msg);
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
	public error user_cnt_by_dom( int dom_id, IntHolder cnt )
			throws null_error, db_error, except { try {
		cnt.value = 0;

		PreparedStatement st = null;
		ResultSet res = null;

		try {
			st = con.prepareStatement( 
				"SELECT count FROM vq_view_user_cnt_by_dom WHERE id_domain=?" );
			int idx=1;
			st.setInt(idx++, dom_id);
			res = st.executeQuery();
			cnt.value = res.next() ? res.getInt(1) : 0;
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
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
	public error user_ls_by_dom( int dom_id, user_info_listHolder uis )
			throws null_error, db_error, except { try {
		uis.value = new user_info[0];
		PreparedStatement st = null;
		ResultSet res = null;
		ArrayList auis = new ArrayList();

		try {
			st = con.prepareStatement( "SELECT pass,dir,flags,login FROM vq_view_user_get "
				+ "WHERE id_domain=? ORDER BY login" );
			int idx=1;
			st.setInt(idx++, dom_id);
			res = st.executeQuery();

			for( idx=1; res.next(); idx=1) {
				user_info ui = new user_info();
				
				ui.pass = res.getString(idx++);
				if( res.wasNull() ) ui.pass = "";
				ui.dir = res.getString(idx++);
				if( res.wasNull() ) ui.dir = "";
				ui.flags = res.getShort(idx++);
				if( res.wasNull() ) ui.flags = 0;
				ui.login = res.getString(idx++);
				if( res.wasNull() ) ui.login = "";
				ui.id_domain = dom_id;
				auis.add(ui);
			}

			uis.value = new user_info [auis.size()];
			for(int i=0, s=auis.size(); i<s; ++i) 
				uis.value[i] = (user_info) auis.get(i);
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
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
	public error eb_add( String re_domain, String re_login ) 
			throws null_error, db_error, except { try {
		String func = "EB_ADD";
		CallableStatement call = con.prepareCall("{ ? = call "+func+"(?, ?) }");
		
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setString(idx++, re_domain);
		call.setString(idx++, re_login);
		call.execute();

		int res = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {}

		if( wasNull ) {
			return lr(err_code.err_func_res, func);
		}

		if( res < 0 ) {
			return res == -1 
				? lr(err_code.err_exists, "") 
				: lr(err_code.err_func_res, func);
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
	public error eb_rm( String re_domain, String re_login ) 
			throws null_error, db_error, except { try {
		String func = "EB_RM";
		CallableStatement call = con.prepareCall("{? = call "+func+"(?, ?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setString(idx++, re_domain);
		call.setString(idx++, re_login);
		call.execute();

		int res = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {}

		if( wasNull ) {
			return lr(err_code.err_func_res, func);
		}

		if( 0 == res )
			return lr(err_code.err_no, "");
			
		return lr(err_code.err_func_res, func);
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
	public error eb_ls( email_banned_listHolder ebs )
			throws db_error, except { try {
		ebs.value = new email_banned[0];

		Statement st = null;
		ResultSet res = null;
		ArrayList aebs = new ArrayList();

		try {
			st = con.createStatement();
			res = st.executeQuery( "SELECT re_domain,re_login FROM vq_view_eb_ls ORDER BY re_domain,re_login" );
			for( int idx=1; res.next(); idx=1 ) {
				email_banned eb = new email_banned();
				eb.re_domain = res.getString(idx++);
				if( res.wasNull() ) eb.re_domain = "";
				eb.re_login = res.getString(idx++);
				if( res.wasNull() ) eb.re_login = "";
				aebs.add(eb);
			}
			ebs.value = new email_banned [aebs.size()];
			for(int i=0, s=aebs.size(); i<s; ++i )
				ebs.value[i] = (email_banned) aebs.get(i);
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
		}
		return lr(err_code.err_no, "");
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }

	/**
	 * 
	 */
	public error user_get( user_infoHolder ai )
			throws null_error, db_error, except { try {
		PreparedStatement st = null;
		ResultSet res = null;
		String ec_msg = "";
		err_code ec = err_code.err_no;

		try {
			st = con.prepareStatement( 
				"SELECT pass,dir,flags FROM vq_view_user_get"
				+" WHERE id_domain=? AND login=?" );
			int idx=1;
			st.setInt(idx++, ai.value.id_domain);
			st.setString(idx++, ai.value.login.toLowerCase());
			res = st.executeQuery();

			if( res.next() ) {
				idx = 1;
				ai.value.pass = res.getString(idx++);
				if( res.wasNull() ) ai.value.pass = "";
				ai.value.dir = res.getString(idx++);
				if( res.wasNull() ) ai.value.dir = "";
				ai.value.flags = res.getShort(idx++);
				if( res.wasNull() ) ai.value.flags = 0;
			} else {
				ec = err_code.err_noent;
				ec_msg = "";
			}
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
		}
		return lr(ec, ec_msg);
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
	public error user_conf_add( int dom_id, String login, String pfix, 
			user_conf_infoHolder ui )
			throws null_error, db_error, except { try {
		login = login.toLowerCase();
		String func = "USER_CONF_ADD";
		CallableStatement call = con.prepareCall("{? = call "+func+"(?,?,?,?,?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setInt(idx++, dom_id);
		call.setString(idx++, login);
		call.setString(idx++, pfix.toLowerCase());
		call.setShort(idx++, ui.value.type);
		call.setString(idx++, ui.value.val);
		call.execute();

		ui.value.id_conf = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {}

		if( wasNull )
			return lr(err_code.err_func_res, func);

		if( ui.value.id_conf < 0 ) {
			switch(ui.value.id_conf) {
			case -1:
				return lr(err_code.err_noent, Integer.toString(dom_id));
			case -2:
				return lr(err_code.err_noent, login);
			default:
				return lr(err_code.err_func_res, func);
			}
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
	public error user_conf_ls(int dom_id, String login, String pfix, 
			user_conf_info_listHolder uis)
			throws null_error, db_error, except { try {
		
		PreparedStatement st = null;
		ResultSet res = null;
		ArrayList auis = new ArrayList();

		try {
			st = con.prepareStatement( 
				"SELECT id_conf,val,type FROM vq_view_user_conf_ls "
				+ "WHERE id_domain=? AND login=? AND (ext=? OR (?=1 AND ext IS NULL))" );

			int idx=1;
			st.setInt(idx++, dom_id);
			st.setString(idx++, login.toLowerCase());
			st.setString(idx++, pfix.toLowerCase());
			st.setInt(idx++, pfix.length() == 0 ? 1 : 0 );
			res = st.executeQuery();

			for( idx=1; res.next(); idx=1 ) {
				user_conf_info uci = new user_conf_info();

				uci.id_conf = res.getInt(idx++);
				if( res.wasNull() ) uci.id_conf = 0;
				uci.val = res.getString(idx++);
				if( res.wasNull() ) uci.val = "";
				uci.type = res.getShort(idx++);
				if( res.wasNull() ) uci.type = 0;
				auis.add(uci);
			}

			uis.value = new user_conf_info [auis.size()];
			for(int i=0, s=auis.size(); i<s; ++i )
				uis.value[i] = (user_conf_info) auis.get(i);
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
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
	public error user_conf_ls_by_type(int dom_id, String login, 
			String pfix, short ut, user_conf_info_listHolder uis)
			throws null_error, db_error, except { try {
		uis.value = new user_conf_info[0];	
		PreparedStatement st = null;
		ResultSet res = null;
		ArrayList auis = new ArrayList();

		try {
			st = con.prepareStatement( 
				"SELECT id_conf,val,type FROM vq_view_user_conf_ls "
				+ "WHERE id_domain=? AND login=? AND "
				+ "(ext=? OR (?=1 AND ext IS NULL)) AND type=?" );
 
			int idx=1;
			st.setInt(idx++, dom_id);
			st.setString(idx++, login.toLowerCase());
			st.setString(idx++, pfix.toLowerCase());
			st.setInt(idx++, pfix.length() == 0 ? 1 : 0 );
			st.setShort(idx++, ut );
			res = st.executeQuery();

			for( idx=1; res.next(); idx=1 ) {
				user_conf_info uci = new user_conf_info();

				uci.id_conf = res.getInt(idx++);
				if( res.wasNull() ) uci.id_conf = 0;
				uci.val = res.getString(idx++);
				if( res.wasNull() ) uci.val = "";
				uci.type = res.getShort(idx++);
				if( res.wasNull() ) uci.type = 0;
				auis.add(uci);
			}

			uis.value = new user_conf_info [auis.size()];
			for(int i=0, s=auis.size(); i<s; ++i )
				uis.value[i] = (user_conf_info) auis.get(i);
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
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
	public error user_conf_rm(int user_conf_id) 
			throws null_error, db_error, except {
		return func_rm(user_conf_id, "USER_CONF_RM");
	}

	/**
	 *
	 */
	public error user_conf_rm_by_type(int dom_id, String login, 
			String ext, short ut)
			throws null_error, db_error, except { try {

		String func = "USER_CONF_RM_BY_TYPE";
		CallableStatement call = con.prepareCall("{? = call "+func+"(?,?,?,?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setInt(idx++, dom_id);
		call.setString(idx++, login.toLowerCase());
		call.setString(idx++, ext.toLowerCase());
		call.setShort(idx++, ut);
		call.execute();

		int res = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {}
		
		if( wasNull )
			return lr(err_code.err_func_res, func);

		if( 0 == res )
			return lr(err_code.err_no, "");
			
		return lr(err_code.err_func_res, func);
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
	public error user_conf_get( user_conf_infoHolder ui )
			throws null_error, db_error, except { try {
		PreparedStatement st = null;
		ResultSet res = null;
		err_code ec = err_code.err_no;
		String ec_msg = "";

		try {
			st = con.prepareStatement( 
				"SELECT TYPE,VAL FROM vq_view_USER_CONF_GET WHERE id_conf=?");
			int idx=1;
			st.setInt(idx++, ui.value.id_conf);
			res = st.executeQuery();

			if( res.next() ) {
				idx = 1;
				ui.value.type = res.getShort(idx++);
				if( res.wasNull() ) ui.value.type = 0;
				ui.value.val = res.getString(idx++);
				if( res.wasNull() ) ui.value.val = "";
			} else {
				ec = err_code.err_noent;
				ec_msg = "";
			}
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
		}
		return lr(ec, ec_msg);
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
	public error user_conf_rep( user_conf_info ui )
			throws null_error, db_error, except { try {
	
		String func = "USER_CONF_REP";
		CallableStatement call = con.prepareCall("{ ? = call "+func+"(?, ?, ?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setInt(idx++, ui.id_conf);
		call.setShort(idx++, ui.type);
		call.setString(idx++, ui.val);
		call.execute();
		
		int res = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {}
		
		if( wasNull )
			return lr(err_code.err_func_res, func);

		if( res < 0) {
			switch( res ) {
			case -1:
				return lr(err_code.err_noent, "");
			default:
				return lr(err_code.err_func_res, func);
			} 
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
	public error user_conf_type_has( int dom_id, String login, 
			String pfix, short ut )
			throws null_error, db_error, except { try {

		PreparedStatement st = null;
		ResultSet res = null;
		err_code ec = err_code.err_noent;
		String ec_msg = "";

		try {
			st = con.prepareStatement( 
				"SELECT COUNT(*) FROM vq_view_user_conf_type_has WHERE "
				+"id_domain=? AND login=? AND (ext=? OR (?=1 AND ext IS NULL))"
				+" AND type=?" );
			int idx=1;
			st.setInt(idx++, dom_id);
			st.setString(idx++, login.toLowerCase());
			st.setString(idx++, pfix.toLowerCase());
			st.setInt(idx++, pfix.length() == 0 ? 1 : 0 );
			st.setShort(idx++, ut );
			res = st.executeQuery();
			while(res.next()) {
				ec = res.getInt(1) != 0
					? err_code.err_no : err_code.err_noent;
				ec_msg = "";
			}
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
		}
		return lr(ec, ec_msg);
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
	public error user_conf_type_cnt( int dom_id, String login, 
			String pfix, short ut, IntHolder cnt )
			throws null_error, db_error, except { try {
		cnt.value = 0;

		PreparedStatement st = null;
		ResultSet res = null;

		try {
			st = con.prepareStatement( 
				"SELECT count FROM vq_view_user_conf_type_cnt WHERE "
				+"id_domain=? AND login=? AND (ext=? OR (?=1 AND ext IS NULL))"
				+" AND type=?" );
			int idx=1;
			st.setInt(idx++, dom_id);
			st.setString(idx++, login.toLowerCase());
			st.setString(idx++, pfix.toLowerCase());
			st.setInt(idx++, pfix.length() == 0 ? 1 : 0 );
			st.setShort(idx++, ut );
			res = st.executeQuery();
			cnt.value = res.next() ? res.getInt(1) : 0;
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
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
	public error qt_user_def_set( int dom_id, int bytes_max, int files_max )
			throws null_error, db_error, except { try {
		String func = "QT_USER_DEF_SET";
		CallableStatement call = con.prepareCall("{? = call "+func+"(?,?,?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setInt(idx++, dom_id);
		call.setInt(idx++, bytes_max);
		call.setInt(idx++, files_max);
		call.execute();

		int res = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {}
		
		if( wasNull )
			return lr(err_code.err_func_res, func);

		if( 0 == res )
			return lr(err_code.err_no, "");
			
		return lr(err_code.err_func_res, func);
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( NullPointerException e ) {
		throw new null_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }

	/**
	 * Gets default quota for new users and sends it via cso
	 * \param dom domain
	 */
	public error qt_user_def_get( int dom_id,
			IntHolder bytes_max, IntHolder files_max )
			throws null_error, db_error, except { try {
		bytes_max.value = 0;
		files_max.value = 0;

		PreparedStatement st = null;
		ResultSet res = null;

		try {
			st = con.prepareStatement( 
				"SELECT qt_user_bytes_def,qt_user_files_def "
				+"FROM vq_view_QT_USER_DEF_GET WHERE id_domain=?" );

			int idx=1;
			st.setInt(idx++, dom_id);
			res = st.executeQuery();
			idx = 1;
			if( res.next() ) {
				bytes_max.value = res.getInt(idx++);
				files_max.value = res.getInt(idx++);
			}
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
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
	 * Gets limits for specified user
	 */
	public error qt_user_get( int dom_id, String login,
			IntHolder bytes_max, IntHolder files_max )
			throws null_error, db_error, except { try {
		bytes_max.value = 0;
		files_max.value = 0;

		PreparedStatement st = null;
		ResultSet res = null;

		try {
			st = con.prepareStatement( 
				"SELECT qt_bytes_max,qt_files_max FROM vq_view_QT_USER_GET"
				+" WHERE id_domain=? AND login=?" );

			int idx=1;
			st.setInt(idx++, dom_id);
			st.setString(idx++, login.toLowerCase());
			res = st.executeQuery();
			idx = 1;
			if( res.next() ) {
				bytes_max.value = res.getInt(idx++);
				files_max.value = res.getInt(idx++);
			}
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
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
	 * Sets limits for specified user
	 */
	public error qt_user_set( int dom_id, String login,
			int bytes_max, int files_max )
			throws null_error, db_error, except { try {

		String func = "QT_USER_SET";
		CallableStatement call = con.prepareCall("{? = call "+func+"(?,?,?,?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setInt(idx++, dom_id);
		call.setString(idx++, login.toLowerCase());
		call.setInt(idx++, bytes_max);
		call.setInt(idx++, files_max);
		call.execute();

		int res = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {}
		
		if( wasNull )
			return lr(err_code.err_func_res, func);

		if( 0 == res )
			return lr(err_code.err_no, "");
			
		return lr(err_code.err_func_res, func);
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( NullPointerException e ) {
		throw new null_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }

	/**
	 * Common part for user_rm, and other functions
	 */
	protected error func_rm( int dom_id, String func )
			throws null_error, db_error, except { try {
		CallableStatement call = con.prepareCall("{? = call "+func+"(?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setInt(idx++, dom_id);
		call.execute();

		int res = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {}
	
		if( wasNull )
			return lr(err_code.err_func_res, func);

		if( 0 == res )
			return lr(err_code.err_no, "");
			
		return lr(err_code.err_func_res, func);
	} catch( SQLException e ) {
		throw new db_error(e.getMessage(), getClass().getName(), 0);
	} catch( NullPointerException e ) {
		throw new null_error(e.getMessage(), getClass().getName(), 0);
	} catch( Exception e ) {
		throw new except(e.getMessage(), getClass().getName(), 0);
	} }

	/**
	 * Common part for da_add and dip_add
	 */
	protected error da_dip_add( int dom_id, String rea, String func )
			throws null_error, db_error, except { try {
		CallableStatement call = con.prepareCall("{ ? = call "+func+"(?, ?) }");
		
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setInt(idx++, dom_id);
		call.setString(idx++, rea);
		call.execute();

		int res = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {}

		if( wasNull )
			return lr(err_code.err_func_res, func);

		if( res < 0 ) {
			return res == -1 
				? lr(err_code.err_exists, Integer.toString(dom_id)) 
				: lr(err_code.err_func_res, func);
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
	 * Common part for da_rm and dip_rm
	 */
	protected error da_dip_rm( String rea, String func )
			throws null_error, db_error, except { try {
		CallableStatement call = con.prepareCall("{ ? = call "+func+"(?) }");
		
		int idx=1;
		call.registerOutParameter(idx++, Types.INTEGER);
		call.setString(idx++, rea);
		call.execute();

		int res = call.getInt(1);
		boolean wasNull = call.wasNull();
		try { call.close(); } catch( Exception e ) {}

		if( wasNull || 0 != res )
			return lr(err_code.err_func_res, func);

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

	protected Connection con = null;
}
