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
#include "cpgsqlauth.hpp"

#include <vqmain.hpp>
#include <conf.hpp>

// CORBA
#include <import_export.h>

#include <getopt.h>

#include <memory>
#include <iostream>
#include <fstream>

using namespace std;
using namespace POA_vq;

void usage( const char * me ) {
	cerr<<"usage: "<<me<<" [ORB options] base_dir"<<endl;
}

int vqmain(int ac, char **av) {
	/*
	 * Initialize the ORB
	 */
	
	CORBA::ORB_var orb = CORBA::ORB_init (ac, av);
	
	if( ac < 2 ) {
			usage(*av);
			return 100;
	}

	int opt;
	bool run = true, exp = true;
	const char * exp_ins = "name_service#Auth.iauth";
	
	while( (opt=getopt(ac, av, "e:ErR")) != -1 ) switch(opt) {
	case 'e':
			exp = true;
			exp_ins = optarg;
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
			usage(*av);
			return 111;
	}

	string conf_dir(*(av+1));
	conf_dir += "/etc/iauth/pgsql/";
	conf::clnconf pgsql(conf_dir+"pgsql", "dbname=mail password=mail user=mail");
	
	/*
	 * Obtain a reference to the RootPOA and its Manager
	 */
	
	CORBA::Object_var poaobj = orb->resolve_initial_references ("RootPOA");
	PortableServer::POA_var root_poa = PortableServer::POA::_narrow (poaobj);
	PortableServer::POAManager_var mgr = root_poa->the_POAManager();
	mgr->activate();

	CORBA::PolicyList pl;
	pl.length(1);
	pl[0] = root_poa->create_thread_policy(PortableServer::SINGLE_THREAD_MODEL);
	PortableServer::POA_var poa = root_poa->create_POA("RealPOA", mgr, pl);

	/*
	 * Create authorization object
	 */
	auto_ptr<cpgsqlauth> pgauth(new cpgsqlauth(pgsql.val_str().c_str()));

	PortableServer::ObjectId_var oid = poa->activate_object (pgauth.get());
	CORBA::Object_var ref = poa->id_to_reference (oid.in());

	if( exp ) {
			try {
					corbautil::exportObjRef(orb, ref, exp_ins);
			} catch( corbautil::ImportExportException & e ) {
					cerr<<e<<endl;
					run = false;
			}
	}
	pgauth->_remove_ref(); // ???
	if( run ) {
			
			/*
			 * Activate the POA and start serving requests
			 */
			cout << "Running." << endl;
#if 1
			orb->run();
#else
			for( ;; ) {
					omni_thread::sleep(1);
					if( orb->work_pending() ) {
							orb->perform_work();
					}
			}
#endif 
	}
	orb->destroy();
	return 0;
}
