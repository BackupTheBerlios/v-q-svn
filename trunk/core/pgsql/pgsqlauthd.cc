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
#include "cpgsqlauth.hpp"

#include "main.hpp"

// CORBA
#include <coss/CosNaming.h>

#include <memory>
#include <iostream>
#include <fstream>

using namespace std;
using namespace POA_vq;

int cppmain(int ac, char **av) {
	/*
	 * Initialize the ORB
	 */
	
	CORBA::ORB_var orb = CORBA::ORB_init (ac, av);
	
	/*
	 * Obtain a reference to the RootPOA and its Manager
	 */
	
	CORBA::Object_var poaobj = orb->resolve_initial_references ("RootPOA");
	PortableServer::POA_var poa = PortableServer::POA::_narrow (poaobj);
	PortableServer::POAManager_var mgr = poa->the_POAManager();
	
	/*
	 * Create authorization object
	 */
	auto_ptr<cpgsqlauth> pgauth(new cpgsqlauth("pgsql.conf"));
	
	/*
	 * Activate the Servant
	 */
	
	PortableServer::ObjectId_var oid = poa->activate_object (pgauth.get());
	CORBA::Object_var ref = poa->id_to_reference (oid.in());
	

	CORBA::Object_var nsobj = orb->resolve_initial_references ("NameService");

	CosNaming::NamingContext_var nc =
			CosNaming::NamingContext::_narrow (nsobj);

	if (CORBA::is_nil (nc)) {
			cerr << "oops, I cannot access the Naming Service!" << endl;
			return 100;
	}

	/*
	 * Construct Naming Service name for our Bank
	 */
	CosNaming::Name name;
	name.length (1);
	name[0].id = CORBA::string_dup ("vq::iauth");
	name[0].kind = CORBA::string_dup ("");

    /*
	 * Store a reference to our Bank in the Naming Service. We use 'rebind'
	 * here instead of 'bind', because rebind does not complain if the desired
	 * name "Bank" is already registered, but silently overwrites it (the
	 * existing reference is probably from an old incarnation of this server).
	 */
	cout << "Binding vq::iauth in the Naming Service ... " << flush;
	nc->rebind (name, ref);
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
