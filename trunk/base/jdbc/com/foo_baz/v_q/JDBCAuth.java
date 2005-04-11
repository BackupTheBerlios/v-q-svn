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
import org.omg.CORBA.*;
import sun.net.util.IPAddressUtil;

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
	public error dom_add( String dom, StringHolder dom_id )
			throws null_error, db_error, except { try {
		dom_id.value = "";

		dom = dom.toLowerCase();

		CallableStatement call = con.prepareCall("{? = dom_add(?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.VARCHAR);
		call.setString(idx++, dom);
		call.execute();

		dom_id.value = call.getString(1);

		if( call.wasNull() || dom_id.value.length() == 0 )
			return lr(err_code.err_func_res, "DOM_ADD");

		if( dom_id.value.charAt(0) == '-' ) {
			return ( dom_id.value.length() >= 2 && dom_id.value.charAt(1) == '1' ) 
				? lr(err_code.err_exists, dom) 
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
	public error dom_id( String dom, StringHolder dom_id )
			throws null_error, db_error, except { try {
		dom_id.value = "";
		dom = dom.toLowerCase();

		PreparedStatement st = null;
		ResultSet res = null;

		try {
			st = con.prepareStatement( 
				"SELECT id_domain FROM vq_view_DOM_ID WHERE domain = ?" );
			st.setString(1, dom);
			res = st.executeQuery();
			while(res.next()) {
				dom_id.value = res.getString(1);
				if( res.wasNull() ) return lr(err_code.err_noent, dom);
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
	*/
	public error dom_name( String dom_id, StringHolder domain )
			throws null_error, db_error, except { try {
		domain.value = "";

		PreparedStatement st = null;
		ResultSet res = null;

		try {
			st = con.prepareStatement( 
				"SELECT domain FROM vq_view_DOM_NAME WHERE id_domain = ?" );
			st.setString(1, dom_id);
			res = st.executeQuery();
			while(res.next()) {
				domain.value = res.getString(1);
				if( res.wasNull() ) return lr(err_code.err_noent, dom_id);
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
	*/
	public error dom_rm( String dom_id ) 
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
			res = st.executeQuery("SELECT id_domain,domain FROM vq_view_dom_ls");

			for( int idx=1; res.next(); idx=1) {
				domain_info di = new domain_info();
				di.id_domain = res.getString(idx++);
				if( res.wasNull() ) di.id_domain = "";
				di.domain = res.getString(idx++);
				if( res.wasNull() ) di.domain = "";
				adis.add(di);
			}

			dis.value = (domain_info []) adis.toArray();
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
		CallableStatement call = con.prepareCall("{ ? = user_add(?, ?, ?, ?, ?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.VARCHAR);
		call.setString(idx++, ai.id_domain);
		call.setString(idx++, ai.login.toLowerCase());
		call.setString(idx++, ai.pass);
		call.setInt(idx++, ai.flags);
		call.setBoolean(idx++, is_banned);
		call.execute();
		
		String res = call.getString(1);
		if(call.wasNull() || res.length() == 0 )
			return lr(err_code.err_func_res, "USER_ADD");

		if( res.charAt(0) == '-' ) {
			if( res.length() == 1 ) 
				return lr(err_code.err_func_res, "USER_ADD");
				
			switch( res.charAt(1) ) {
			case '2':
				return lr(err_code.err_user_inv, "");
			case '1':
				return lr(err_code.err_exists, "");
			default:
				return lr(err_code.err_func_res, "USER_ADD");
			} 
		}

		if( "0".equals(res) )
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
	public error da_add( String dom_id, String ali ) 
			throws null_error, db_error, except {
		return da_dip_add(dom_id, ali, "DA_ADD");
	}

	/**
	 *
	 */
	public error dip_add( String dom_id, String ali ) 
			throws null_error, db_error, except {

		if( IPAddressUtil.textToNumericFormatV4(ali) == null
			&& IPAddressUtil.textToNumericFormatV6(ali) == null )
			return lr(err_code.err_dom_inv, ali);

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
	public error da_ls_by_dom( String dom_id, string_listHolder reas ) 
			throws null_error, db_error, except {
		return da_dip_ls_by_dom( dom_id, reas, "alias", "vq_view_DA_LS" );
	}

	/**
	 *
	 */
	public error dip_ls_by_dom( String dom_id, string_listHolder reas ) 
			throws null_error, db_error, except {
		return da_dip_ls_by_dom( dom_id, reas, "ip", "vq_view_DIP_LS" );
	}

	/**
	 *
	 */
	public error da_dip_ls_by_dom( String dom_id, string_listHolder reas, 
			String field, String view )
			throws null_error, db_error, except { try {

		reas.value = new String[0];
		PreparedStatement st = null;
		ResultSet res = null;
		ArrayList areas = new ArrayList();

		try {
			st = con.prepareStatement(
				"SELECT "+field+" FROM "+view+" WHERE id_domain=?");
			st.setString(1, dom_id);
			res = st.executeQuery();

			while( res.next() ) {
				String item = res.getString(1);
				if( res.wasNull() ) item = "";
				areas.add(item);
			}

			reas.value = (String []) areas.toArray();
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
	public error user_pass( String dom_id, String login, String pass ) 
			throws null_error, db_error, except { try {
		
		CallableStatement call = con.prepareCall("{ ? = user_pass(?, ?, ?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.VARCHAR);
		call.setString(idx++, dom_id);
		call.setString(idx++, login.toLowerCase());
		call.setString(idx++, pass);
		call.execute();
		
		String res = call.getString(1);
		if(call.wasNull() || res.length() == 0 )
			return lr(err_code.err_func_res, "USER_PASS");

		if( res.charAt(0) == '-' ) {
			if( res.length() == 1 ) 
				return lr(err_code.err_func_res, "USER_ADD");
				
			switch( res.charAt(1) ) {
			case '1':
				return lr(err_code.err_exists, "");
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
	public error user_rm( String dom_id, String login ) 
			throws null_error, db_error, except {
		return user_eb_rm(dom_id, login, "USER_RM");
	}

	/**
	*/
	public error user_ex( String dom_id, String login )
			throws null_error, db_error, except { try {
		PreparedStatement st = null;
		ResultSet res = null;

		try {
			st = con.prepareStatement( 
				"SELECT EXISTS (SELECT * FROM vq_view_USER_EX "
				+ "WHERE id_domain=? AND login=?)" );
			int idx=1;
			st.setString(idx++, dom_id);
			st.setString(idx++, login.toLowerCase());
			res = st.executeQuery();
			while(res.next()) {
				return lr( res.getBoolean(1) 
					? err_code.err_noent : err_code.err_no, "" );
			}
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
		}
		return lr(err_code.err_noent, "");
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
			throws null_error, db_error, except {
		return da_dip_add( re_domain, re_login, "EB_ADD" );
	}

	/**
	 *
	 */
	public error eb_rm( String re_domain, String re_login ) 
			throws null_error, db_error, except {
		return user_eb_rm(re_domain, re_login, "EB_RM");
	}

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
			res = st.executeQuery( "SELECT re_domain,re_login FROM vq_view_eb_ls" );
			for( int idx=1; res.next(); idx=1 ) {
				email_banned eb = new email_banned();
				eb.re_domain = res.getString(idx++);
				if( res.wasNull() ) eb.re_domain = "";
				eb.re_login = res.getString(idx++);
				if( res.wasNull() ) eb.re_login = "";
				aebs.add(eb);
			}
			ebs.value = (email_banned []) aebs.toArray();
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

		try {
			st = con.prepareStatement( 
				"SELECT pass,dir,flags FROM vq_view_user_get"
				+" WHERE id_domain=? AND login=?" );
			int idx=1;
			st.setString(idx++, ai.value.id_domain);
			st.setString(idx++, ai.value.login.toLowerCase());
			res = st.executeQuery();

			if( ! res.next() )
				return lr(err_code.err_noent, "");

			idx = 1;
			ai.value.pass = res.getString(idx++);
			if( res.wasNull() ) ai.value.pass = "";
			ai.value.dir = res.getString(idx++);
			if( res.wasNull() ) ai.value.dir = "";
			ai.value.flags = res.getShort(idx++);
			if( res.wasNull() ) ai.value.flags = 0;
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
	public error user_conf_add( String dom_id, String login, String pfix, 
			user_conf_infoHolder ui )
			throws null_error, db_error, except { try {

		String func = "USER_CONF_ADD";
		CallableStatement call = con.prepareCall("{? = "+func+"(?, ?, ?, ?, ?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.VARCHAR);
		call.setString(idx++, dom_id);
		call.setString(idx++, login.toLowerCase());
		call.setString(idx++, pfix.toLowerCase());
		call.setShort(idx++, ui.value.type);
		call.setString(idx++, ui.value.val);
		call.execute();

		ui.value.id_conf = call.getString(1);

		if( call.wasNull() || ui.value.id_conf.length() == 0 )
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
	public error user_conf_ls(String dom_id, String login, String pfix, 
			user_conf_info_listHolder uis)
			throws null_error, db_error, except { try {
		
		PreparedStatement st = null;
		ResultSet res = null;
		ArrayList auis = new ArrayList();

		try {
			st = con.prepareStatement( 
				"SELECT id_conf,val,type FROM vq_view_user_conf_ls "
				+ "WHERE id_domain=? AND login=? AND ext=?" );

			int idx=1;
			st.setString(idx++, dom_id);
			st.setString(idx++, login.toLowerCase());
			st.setString(idx++, pfix.toLowerCase());
			res = st.executeQuery();

			for( idx=1; res.next(); idx=1 ) {
				user_conf_info uci = new user_conf_info();

				uci.id_conf = res.getString(idx++);
				if( res.wasNull() ) uci.id_conf = "";
				uci.val = res.getString(idx++);
				if( res.wasNull() ) uci.val = "";
				uci.type = res.getShort(idx++);
				if( res.wasNull() ) uci.type = 0;
				auis.add(uci);
			}

			uis.value = (user_conf_info []) auis.toArray();
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
	public error user_conf_ls_by_type(String dom_id, String login, 
			String pfix, short ut, user_conf_info_listHolder uis)
			throws null_error, db_error, except { try {
		uis.value = new user_conf_info[0];	
		PreparedStatement st = null;
		ResultSet res = null;
		ArrayList auis = new ArrayList();

		try {
			st = con.prepareStatement( 
				"SELECT id_conf,val,type FROM vq_view_user_conf_ls "
				+ "WHERE id_domain=? AND login=? AND ext=? AND type=?" );
 
			int idx=1;
			st.setString(idx++, dom_id);
			st.setString(idx++, login.toLowerCase());
			st.setString(idx++, pfix.toLowerCase());
			st.setShort(idx++, ut );
			res = st.executeQuery();

			for( idx=1; res.next(); idx=1 ) {
				user_conf_info uci = new user_conf_info();

				uci.id_conf = res.getString(idx++);
				if( res.wasNull() ) uci.id_conf = "";
				uci.val = res.getString(idx++);
				if( res.wasNull() ) uci.val = "";
				uci.type = res.getShort(idx++);
				if( res.wasNull() ) uci.type = 0;
				auis.add(uci);
			}

			uis.value = (user_conf_info []) auis.toArray();
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
	public error user_conf_rm(String user_conf_id) 
			throws null_error, db_error, except {
		return func_rm(user_conf_id, "USER_CONF_RM");
	}

	/**
	 *
	 */
	public error user_conf_rm_by_type(String dom_id, String login, 
			String ext, short ut)
			throws null_error, db_error, except { try {

		String func = "USER_CONF_RM_BY_TYPE";
		CallableStatement call = con.prepareCall("{? = "+func+"(?,?,?,?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.VARCHAR);
		call.setString(idx++, dom_id);
		call.setString(idx++, login.toLowerCase());
		call.setString(idx++, ext.toLowerCase());
		call.setShort(idx++, ut);
		call.execute();

		String res = call.getString(1);
		if( call.wasNull() || res.length() == 0 )
			return lr(err_code.err_func_res, func);

		if( "0".equals(res) )
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

		try {
			st = con.prepareStatement( 
				"SELECT TYPE,VAL FROM vq_view_USER_CONF_GET WHERE id_conf=?");
			int idx=1;
			st.setString(idx++, ui.value.id_conf);
			res = st.executeQuery();

			if( ! res.next() )
				return lr(err_code.err_noent, "");

			idx = 1;
			ui.value.type = res.getShort(idx++);
			if( res.wasNull() ) ui.value.type = 0;
			ui.value.val = res.getString(idx++);
			if( res.wasNull() ) ui.value.val = "";
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
	public error user_conf_rep( user_conf_info ui )
			throws null_error, db_error, except { try {
	
		String func = "USER_CONF_REP";
		CallableStatement call = con.prepareCall("{ ? = "+func+"(?, ?, ?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.VARCHAR);
		call.setString(idx++, ui.id_conf);
		call.setShort(idx++, ui.type);
		call.setString(idx++, ui.val);
		call.execute();
		
		String res = call.getString(1);
		if(call.wasNull() || res.length() == 0 )
			return lr(err_code.err_func_res, func);

		if( res.charAt(0) == '-' ) {
			if( res.length() == 1 ) 
				return lr(err_code.err_func_res, func);
				
			switch( res.charAt(1) ) {
			case '1':
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
	public error user_conf_type_has( String dom_id, String login, 
			String pfix, short ut )
			throws null_error, db_error, except { try {

		PreparedStatement st = null;
		ResultSet res = null;

		try {
			st = con.prepareStatement( 
				"SELECT EXISTS (SELECT * FROM vq_view_user_conf_type_has WHERE "
				+"id_domain=? AND login=? AND ext=? AND type=?" );
			int idx=1;
			st.setString(idx++, dom_id);
			st.setString(idx++, login.toLowerCase());
			st.setString(idx++, pfix.toLowerCase());
			st.setShort(idx++, ut );
			res = st.executeQuery();
			while(res.next()) {
				return lr( res.getBoolean(1) 
					? err_code.err_noent : err_code.err_no, "" );
			}
		} finally {
			try { if(res != null) res.close(); } catch(Exception e) {}
			try { if(st != null) st.close(); } catch(Exception e) {}
		}
		return lr(err_code.err_noent, "");
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
	public error user_conf_type_cnt( String dom_id, String login, 
			String pfix, short ut, IntHolder cnt )
			throws null_error, db_error, except { try {
		cnt.value = 0;

		PreparedStatement st = null;
		ResultSet res = null;

		try {
			st = con.prepareStatement( 
				"SELECT count FROM vq_view_user_conf_type_cnt WHERE "
				+"id_domain=? AND login=? AND ext=? AND type=?" );
			int idx=1;
			st.setString(idx++, dom_id);
			st.setString(idx++, login.toLowerCase());
			st.setString(idx++, pfix.toLowerCase());
			st.setShort(idx++, ut );
			res = st.executeQuery();
			cnt.value = res.getInt(1);
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
	public error qt_user_def_set( String dom_id, int bytes_max, int files_max )
			throws null_error, db_error, except { try {
		String func = "QT_USER_DEF_SET";
		CallableStatement call = con.prepareCall("{? = "+func+"(?,?,?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.VARCHAR);
		call.setString(idx++, dom_id);
		call.setInt(idx++, bytes_max);
		call.setInt(idx++, files_max);
		call.execute();

		String res = call.getString(1);
		if( call.wasNull() || res.length() == 0 )
			return lr(err_code.err_func_res, func);

		if( "0".equals(res) )
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
	public error qt_user_def_get( String dom_id,
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
			st.setString(idx++, dom_id);
			res = st.executeQuery();
			idx = 1;
			bytes_max.value = res.getInt(idx++);
			files_max.value = res.getInt(idx++);
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
	public error qt_user_get( String dom_id, String login,
			IntHolder bytes_max, IntHolder files_max )
			throws null_error, db_error, except { try {
		bytes_max.value = 0;
		files_max.value = 0;

		PreparedStatement st = null;
		ResultSet res = null;

		try {
			st = con.prepareStatement( 
				"SELECT qt_bytes_max,qt_files_max FROM vq_view_QT_USER_DEF_GET"
				+" WHERE id_domain=? AND login=?" );

			int idx=1;
			st.setString(idx++, dom_id);
			st.setString(idx++, login.toLowerCase());
			res = st.executeQuery();
			idx = 1;
			bytes_max.value = res.getInt(idx++);
			files_max.value = res.getInt(idx++);
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
	public error qt_user_set( String dom_id, String login,
			int bytes_max, int files_max )
			throws null_error, db_error, except { try {

		String func = "QT_USER_SET";
		CallableStatement call = con.prepareCall("{? = "+func+"(?,?,?,?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.VARCHAR);
		call.setString(idx++, dom_id);
		call.setString(idx++, login.toLowerCase());
		call.setInt(idx++, bytes_max);
		call.setInt(idx++, files_max);
		call.execute();

		String res = call.getString(1);
		if( call.wasNull() || res.length() == 0 )
			return lr(err_code.err_func_res, func);

		if( "0".equals(res) )
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
	protected error func_rm( String dom_id, String func )
			throws null_error, db_error, except { try {
		CallableStatement call = con.prepareCall("{? = "+func+"(?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.VARCHAR);
		call.setString(idx++, dom_id);
		call.execute();

		String res = call.getString(1);
		if( call.wasNull() || res.length() == 0 )
			return lr(err_code.err_func_res, func);

		if( "0".equals(res) )
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
	protected error da_dip_add( String dom_id, String rea, String func )
			throws null_error, db_error, except { try {
		CallableStatement call = con.prepareCall("{ ? = "+func+"(?, ?) }");
		
		int idx=1;
		call.registerOutParameter(idx++, Types.VARCHAR);
		call.setString(idx++, dom_id);
		call.setString(idx++, rea);
		call.execute();

		String res = call.getString(1);

		if( call.wasNull() || res.length() == 0 )
			return lr(err_code.err_func_res, func);

		if( res.charAt(0) == '-' ) {
			return ( res.length() >= 2 && res.charAt(1) == '1' ) 
				? lr(err_code.err_exists, dom_id) 
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
	 * Common part for user_rm and eb_rm.
	 */
	protected error user_eb_rm( String dom_id, String login, String func )
			throws null_error, db_error, except { try {
		CallableStatement call = con.prepareCall("{? = "+func+"(?, ?)}");
		int idx=1;
		call.registerOutParameter(idx++, Types.VARCHAR);
		call.setString(idx++, dom_id);
		call.setString(idx++, login);
		call.execute();

		String res = call.getString(1);
		if( call.wasNull() || res.length() == 0 )
			return lr(err_code.err_func_res, func);

		if( "0".equals(res) )
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
	 * Common part for da_rm and dip_rm
	 */
	protected error da_dip_rm( String rea, String func )
			throws null_error, db_error, except { try {
		CallableStatement call = con.prepareCall("{ ? = "+func+"(?) }");
		
		int idx=1;
		call.registerOutParameter(idx++, Types.VARCHAR);
		call.setString(idx++, rea);
		call.execute();

		String res = call.getString(1);

		if( call.wasNull() || ! "0".equals(res) )
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
