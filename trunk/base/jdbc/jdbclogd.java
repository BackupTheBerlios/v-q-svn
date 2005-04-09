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

public class jdbclogd {

	/**
	 * Prints info about arguments
	 */ 
	void usage() {
		System.out.println("usage: "+getClass().getName()+" [ORB options] base_dir");
	}

	/**
	 * @param args arguments from main
	 */
	int run( String[] args ) {
		ORB orb = ORB.init(args, null);
	
		if( args.length < 1 ) {
				usage();
				System.exit(100);
		}

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

		POAHier poa;
		try {
			poa = new POAHier(orb, "fixed_ports_no_imr", "");
		} catch( PoaUtilityException e ) {
			System.err.println(e.getMessage());
			System.exit(111);
		}

		JDBCLog log = new JDBCLog("");

		byte[] oid = poa.getCorePoa().activate_object(log);
		org.omg.CORBA.Object ref = poa.getCorePoa().id_to_reference(oid);
		log._remove_ref();

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
	public static void main( String[] args ) {
		System.exit(new jdbclogd().run(args));
	}
}
