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
#include "cqmailvq.hpp"

#include <vqmain.hpp>
#include <conf.hpp>

// CORBA
#include <coss/CosNaming.h>

#include <sys/types.h>
#include <unistd.h>

#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace POA_vq;

int vqmain(int ac, char **av) {
	/*
	 * Initialize the ORB
	 */
	
	CORBA::ORB_var orb = CORBA::ORB_init (ac, av);
	
	if( ac < 2 ) {
			cout<<"usage: "<<*av<<" [ORB options] base_dir"<<endl;
			return 100;
	}

	string conf_dir(*(av+1));
	conf_dir += "/etc/ivq/qmail/";
	conf::clnconf ivq_name(conf_dir+"ivq_name", "vq::ivq");
	conf::clnconf iauth_name(conf_dir+"iauth_name", "vq::iauth");
	conf::cintconf split_dom(conf_dir+"split_dom", "1");
	conf::cintconf split_user(conf_dir+"split_user", "3");
	conf::cintconf fmode(conf_dir+"fmode", "0640");
	conf::cintconf mmode(conf_dir+"mmode", "0750");
	conf::cintconf dmode(conf_dir+"dmode", "0750");
	conf::clnconf user(conf_dir+"user", "vq");
	ostringstream os;
	os<<geteuid();
	conf::cuidconf uid(conf_dir+"uid", os.str());
	os.str("");
	os<<getegid();
	conf::cgidconf gid(conf_dir+"gid", os.str());

	/*
	 * Obtain a reference to the RootPOA and its Manager
	 */
	
	CORBA::Object_var poaobj = orb->resolve_initial_references ("RootPOA");
	PortableServer::POA_var poa = PortableServer::POA::_narrow (poaobj);
	PortableServer::POAManager_var mgr = poa->the_POAManager();

	/* 
	 * Get NameService object
	 */
	CORBA::Object_var nsobj = orb->resolve_initial_references ("NameService");

	CosNaming::NamingContext_var nc =
			CosNaming::NamingContext::_narrow (nsobj);

	if (CORBA::is_nil (nc)) {
			cerr << "oops, I cannot access the Naming Service!" << endl;
			return 100;
	}

	/*
	 * Get iauth object
	 */
	CosNaming::Name ianame;
	ianame.length(1);
	ianame[0].id = iauth_name.val_str().c_str();
	ianame[0].kind = static_cast<const char *>("");

	CORBA::Object_var iaobj;
	try {
			iaobj = nc->resolve(ianame);
	} catch(CosNaming::NamingContext::NotFound &) {
			cout<<iauth_name.val_str()<<" was not found"<<endl;
			return 100;
	} catch(CosNaming::NamingContext::CannotProceed &) {
			cout<<"can't proceed with "<<iauth_name.val_str()<<endl;
			return 100;
	} catch(CosNaming::NamingContext::InvalidName &) {
			cout<<iauth_name.val_str()<<" is an invalid name"<<endl;
			return 100;
	}
	vq::iauth_var auth(vq::iauth::_narrow(iaobj));
	if( CORBA::is_nil(auth) ) {
			cout<<"can't narrow "<<iauth_name.val_str()<<endl;
			return 100;
	}

	/*
	 * Create authorization object
	 */
	auto_ptr<cqmailvq> vqimp(new cqmailvq(*(av+1), auth,
		split_dom.val_int(), split_user.val_int(),
		fmode.val_int(), mmode.val_int(), dmode.val_int(),
		user.val_str(), uid.val_int(), gid.val_int() ));
	
	/*
	 * Activate the Servant
	 */
	
	PortableServer::ObjectId_var oid = poa->activate_object (vqimp.get());
	CORBA::Object_var ref = poa->id_to_reference (oid.in());
	

	/*
	 * Construct Naming Service name for our Bank
	 */
	CosNaming::Name name;
	name.length (1);
	name[0].id = CORBA::string_dup (ivq_name.val_str().c_str());
	name[0].kind = CORBA::string_dup ("");

    /*
	 * Store a reference to our Bank in the Naming Service. We use 'rebind'
	 * here instead of 'bind', because rebind does not complain if the desired
	 * name "Bank" is already registered, but silently overwrites it (the
	 * existing reference is probably from an old incarnation of this server).
	 */
	cout << "Binding "<<ivq_name.val_str()<<" in the Naming Service ... " << flush;
	nc->rebind(name, ref);
	cout << "done." << endl;

	/*
	 * Activate the POA and start serving requests
	 */
	
	cout << "Running." << endl;
	
	mgr->activate ();
	orb->run();
	
	/*
	 * Shutdown (never reached)
	 */
	
	poa->destroy (TRUE, TRUE);
	return 0;
}
