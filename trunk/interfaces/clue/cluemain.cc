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

#include <import_export.h>

#include <iostream>
#include <cstdlib>

int vqmain( int ac, char ** av ) {
	using namespace std;

	conf::clnconf ivq_import(VQ_HOME+"/etc/ivq/ivq_import", "name_service#VQ.ivq");
	
	cluemain_env ce;
	ce.orb = CORBA::ORB_init (ac, av);
			
	/*
	 * Obtain a reference to the RootPOA and its Manager
	 */
	CORBA::Object_var poaobj = ce.orb->resolve_initial_references ("RootPOA");
	PortableServer::POA_var poa = PortableServer::POA::_narrow (poaobj);

	CORBA::Object_var ivqobj;
	try {
			ivqobj = corbautil::importObjRef(ce.orb, ivq_import.val_str().c_str());
	} catch( corbautil::ImportExportException & e ) {
			cerr<<e<<endl;
			ce.orb->destroy();
			return 100;
	}

	try {
			ce.vq = vq::ivq::_narrow(ivqobj);
			if( CORBA::is_nil(ce.vq) ) {
					cerr<<"can't narrow "<<ivq_import.val_str()<<endl;
					ce.orb->destroy();
					return 100;
			}
	} catch(...) {
			cerr<<"Exception while resolving ivq implementation"<<endl;
			throw;
	}	

	try {
			int ret = cluemain(ac, av, ce);
			ce.orb->shutdown(0);
			ce.orb->destroy();
			return ret;
	} catch(...) {
			ce.orb->destroy();
			throw;
	}
}
