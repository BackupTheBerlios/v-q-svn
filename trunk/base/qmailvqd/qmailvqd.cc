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
#include "cqmailvq.hpp"

#include <vqmain.hpp>
#include <conf.hpp>

// CORBA
#include <import_export.h>

#include <sys/types.h>
#include <unistd.h>

#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace POA_vq;

void usage(const char * me) {
	cout<<"usage: "<<me<<" [ORB options] base_dir"<<endl;
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
	const char * exp_ins = "name_service#VQ.ivq";
	
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

	string base(*(av+1)); base += '/';
	string conf_dir(base+"etc/ivq/qmail/");
	conf::clnconf iauth_import(conf_dir+"iauth_import", "name_service#Auth.iauth");
	conf::cintconf split_dom(conf_dir+"split_dom", SPLIT_DOM);
	conf::cintconf split_user(conf_dir+"split_user", SPLIT_USER);
	conf::cintconf fmode(conf_dir+"fmode", "0640");
	conf::cintconf mmode(conf_dir+"mmode", "0750");
	conf::cintconf dmode(conf_dir+"dmode", "0750");
	conf::clnconf user(conf_dir+"user", "_vq");
	ostringstream os;
	os<<geteuid();
	conf::cuidconf uid(conf_dir+"uid", os.str());
	os.str("");
	os<<getegid();
	conf::cgidconf gid(conf_dir+"gid", os.str());
	conf::clnconf data(conf_dir+"data", DATA);

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

	CORBA::Object_var iaobj;
	try {
			iaobj = corbautil::importObjRef(orb, iauth_import.val_str().c_str());
	} catch( corbautil::ImportExportException & e ) {
			cerr<<e<<endl;
			orb->destroy();
			return 111;
	}
	vq::iauth_var auth(vq::iauth::_narrow(iaobj));
	if( CORBA::is_nil(auth) ) {
			cout<<"can't narrow iauth implementation"<<endl;
			return 100;
	}

	/*
	 * Create authorization object
	 */
	auto_ptr<cqmailvq> vqimp(new cqmailvq(base, data.val_str()+"/domains", auth,
		split_dom.val_int(), split_user.val_int(),
		fmode.val_int(), mmode.val_int(), dmode.val_int(),
		user.val_str(), uid.val_int(), gid.val_int() ));
	
	/*
	 * Activate the Servant
	 */
	
	PortableServer::ObjectId_var oid = poa->activate_object (vqimp.get());
	CORBA::Object_var ref = poa->id_to_reference (oid.in());

	if( exp ) {
			try {
					corbautil::exportObjRef(orb, ref, exp_ins);
			} catch( corbautil::ImportExportException & e ) {
					cerr<<e<<endl;
					run = false;
			}
	}
	vqimp->_remove_ref();
	if( run ) {
			/*
			 * Activate the POA and start serving requests
			 */
	
			cout << "Running." << endl;
			orb->run();
	}
	
	orb->destroy();
	return 0;
}
