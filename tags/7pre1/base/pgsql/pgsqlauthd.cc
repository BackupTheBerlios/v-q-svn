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
#include <PoaUtility.h>

#include <getopt.h>

#include <memory>
#include <iostream>
#include <fstream>

using namespace std;
using namespace POA_vq;

/**
 *
 */
class cpoa_hier : corbautil::PoaUtility {
		public:
				cpoa_hier( CORBA::ORB_ptr orb, const std::string & depMod,
						const std::string & policy );

				POAManager_ptr core_mgr() { return this->_core_mgr.mgr(); }

				POA_ptr core_poa() { return this->_core_poa; }

		private:
				corbautil::LabelledPOAManager _core_mgr;
				POA_var _core_poa;

				cpoa_hier( const cpoa_hier & );
				cpoa_hier & operator = ( const cpoa_hier & );
};

cpoa_hier::cpoa_hier( CORBA::ORB_ptr orb, const std::string & depMod, 
	const std::string & policy ) 
		: PoaUtility( orb, corbautil::PoaUtility::stringToDeploymentModel(depMod.c_str()) ) {

	_core_mgr = createPoaManager("core_mgr");

	_core_poa = createPoa("core_poa", root(), _core_mgr, policy.c_str());
}

void usage( const char * me ) {
	cerr<<"usage: "<<me<<" [ORB options] [-e export|E] [-r|R]"<<endl;
}

int vqmain(int ac, char **av) {
	/*
	 * Initialize the ORB
	 */
	
	CORBA::ORB_var orb = CORBA::ORB_init (ac, av);
	
	int opt;
	bool run = true, exp = true;
	const char * exp_ins = "name_service#Auth.iauth";
	
	while( (opt=getopt(ac, av, "x:XrR")) != -1 ) switch(opt) {
	case 'x':
			exp = true;
			exp_ins = optarg;
			break;
	case 'X':
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

	string conf_dir(VQ_ETC_DIR + "/iauth/pgsql/");
	conf::clnconf pgsql(conf_dir+"pgsql", "dbname=mail password=mail user=mail");
	conf::clnconf dep_mod(conf_dir+"dep_mod", "fixed_ports_no_imr");
	conf::clnconf policy(conf_dir+"policy", "single_thread_model");
	conf::cintconf pgsql_pool(conf_dir+"pgsql_pool", "1");
	
	auto_ptr<cpoa_hier> poa;
	try {
			poa.reset( new cpoa_hier(orb, dep_mod.val_str(), 
				policy.val_str()) );
	} catch( corbautil::PoaUtilityException & e ) {
			cerr<<e<<endl;
			return 111;
	}
	
	/*
	 * Create authorization object
	 */
	auto_ptr<cpgsqlauth> pgauth(new cpgsqlauth(pgsql.val_str(), 
		pgsql_pool.val_int()));

	PortableServer::ObjectId_var oid = poa->core_poa()->activate_object(pgauth.get());
	CORBA::Object_var ref = poa->core_poa()->id_to_reference (oid.in());
	pgauth->_remove_ref(); // ???

	if( exp ) {
			try {
					corbautil::exportObjRef(orb, ref, exp_ins);
			} catch( corbautil::ImportExportException & e ) {
					cerr<<e<<endl;
					run = false;
			}
	}
	if( run ) {
			cout << "Running." << endl;
			poa->core_mgr()->activate();
			orb->run();
	}
	orb->destroy();
	return 0;
}
