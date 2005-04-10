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
import com.foo_baz.v_q.*;
import com.foo_baz.Getopt;

import com.iona.corbautil.*;

import org.omg.CORBA.*;
import java.util.*;
import javax.naming.*;
import java.sql.*;
import javax.sql.*;

public class jdbclogd {

	/**
	 * Prints info about arguments
	 */ 
	void usage() {
		System.out.println("usage: "+getClass().getName()+" [ORB options]");
	}

	/**
	 * @param args arguments from main
	 */
	int run( String[] args ) throws Exception {
		ORB orb = ORB.init(args, null);
	
		boolean run = true, exp = true;
		String exp_ins = "name_service#Logger.ilogger";
		Getopt go = new Getopt();
		int opt;
		while((opt=go.parse(args, "e:ErR"))!= -1) {
			switch(opt) {
			case 'e':
				exp = true;
				exp_ins = go.optarg;
				break;
			case 'E':
				exp = false;
				break;
			case 'r':
				run = true;
				break;
			case 'R':
				run = false;
				break;
			case '?':
			default:
				usage();
				return 111;
			}
		}

		Properties props = System.getProperties();
		String me = getClass().getName();
		String dep_mod_prop = "com.foo_baz.v_q."+me+".dep_mod";
		String policy_prop = "com.foo_baz.v_q."+me+".policy";
		String ds_prop = "com.foo_baz.v_q."+me+".ds";
		String jdbc_prop = "com.foo_baz.v_q."+me+".jdbc";
		
		String dep_mod = props.getProperty(dep_mod_prop) == null 
			? "fixed_ports_no_imr" : props.getProperty(dep_mod_prop);
		String policy = props.getProperty(policy_prop) == null 
			? "" : props.getProperty(policy_prop);
		String ds = props.getProperty(ds_prop);
		String jdbc = props.getProperty(jdbc_prop);

		System.out.println("Configuration: ");
		System.out.println(dep_mod_prop+": "+dep_mod);
		System.out.println(policy_prop+": "+policy);
		System.out.println(ds_prop+": "+ds);
		System.out.println(jdbc_prop+": "+jdbc);

		POAHier poa = null;
		try {
			poa = new POAHier(orb, dep_mod, policy);
		} catch( PoaUtilityException e ) {
			System.err.println(e.getMessage());
			System.exit(111);
		}

		Connection con = null;
		if( ds != null ) {
			con = ((DataSource) new InitialContext().lookup(ds)).getConnection();
		}
		if( jdbc != null ) {
			con = DriverManager.getConnection(jdbc);
		}

		JDBCLog log = new JDBCLog(con);

		byte[] oid = poa.getCorePoa().activate_object(log);
		org.omg.CORBA.Object ref = poa.getCorePoa().id_to_reference(oid);

		if(exp) {
			try {
				ImportExport.exportObjRef(orb, ref, exp_ins);
			} catch( ImportExportException e ) {
				System.err.println(e.getMessage());
				run = false;
			}
		}

		if(run) {
			System.out.println("Running.");
			poa.getCoreMgr().activate();
			orb.run();
		}

		orb.destroy();
		orb = null;
		return 0;
	}
	
	/**
	 * Entry point
	 * @see void run( String[] args )
	 */
	public static void main( String[] args ) throws Exception {
		System.exit(new jdbclogd().run(args));
	}
}
