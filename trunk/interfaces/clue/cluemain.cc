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
#include "cluemain.hpp"

#include <vqmain.hpp>
#include <conf.hpp>

#include <iostream>
#include <cstdlib>

		
int vqmain( int ac, char ** av ) {
	using namespace std;

	conf::clnconf ivq_name(VQ_HOME+"/etc/ivq/ivq_name", "vq::ivq");
	/*
	 * Initialize the ORB
	 */
	string orb_conf(VQ_HOME+"/etc/ivq/orb_conf");
	char * orb_av[] = {
			*av,
			"-ORBConfFile",
			 const_cast<char *>(orb_conf.c_str())
	};
	int orb_ac = sizeof orb_av/sizeof *orb_av;
	CORBA::ORB_var orb = CORBA::ORB_init (orb_ac, orb_av);
			
	/*
	 * Obtain a reference to the RootPOA and its Manager
	 */
	CORBA::Object_var poaobj = orb->resolve_initial_references ("RootPOA");
	PortableServer::POA_var poa = PortableServer::POA::_narrow (poaobj);
	PortableServer::POAManager_var mgr = poa->the_POAManager();

	cluemain_env ce;

	/* 
	 * Get NameService object
	 */
	CORBA::Object_var nsobj;
	CosNaming::NamingContext_var nc;
	try {
			nsobj = orb->resolve_initial_references ("NameService");
			ce.ns = CosNaming::NamingContext::_narrow (nsobj);
		
			if (CORBA::is_nil (ce.ns)) {
					cerr << "oops, I cannot access the Naming Service!" << endl;
					return 100;
			}
	} corba_catch(" while resolving NameService");
		
	/*
	 * Get iauth object
	 */
	CosNaming::Name obj_name;
	obj_name.length(1);
	obj_name[0].id = ivq_name.val_str().c_str();
	obj_name[0].kind = static_cast<const char *>("");

	CORBA::Object_var ivqobj;
	vq::ivq_var vq;
	try {
			ivqobj = ce.ns->resolve(obj_name);
			ce.vq = vq::ivq::_narrow(ivqobj);
			if( CORBA::is_nil(ce.vq) ) {
					cerr<<"can't narrow "<<ivq_name.val_str()<<endl;
					return 100;
			}
	} corba_catch(" while resolving "+ivq_name.val_str());	
	
	return cluemain(ac, av, ce);
}
