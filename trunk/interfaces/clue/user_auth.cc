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
#include "auto/lmd5.h"
#include "cluemain.hpp"
#include "error2str.hpp"

#include <logger.hpp>
#include <text.hpp>
#include <sys.hpp>
#include <conf.hpp>
#include <vqmain.hpp>

#include <import_export.h>

#include <unistd.h>

#include <iostream>
#include <sstream>

using namespace std;
using namespace vq;
using namespace text;
using namespace sys;
using clue::error2str;

char **av;
int ac;
const char *remip = NULL, *locip = NULL, *defdom = NULL;
bool quiet = false;
string resp, pass, login;
enum { smtp, pop3 } contype;

string atchars("@%");

::vq::ilogger_var olog;
::vq::ivq_var ovq;

::vq::ivq::user_info vqai;
::vq::ivq::error_var ret;

#define LOG(x,y) delete olog->write(x,y)

/*
 *
 */
char read_auth_info() {
	if( ! getline(3, login, '\0')
		|| ! getline(3, pass, '\0')
		|| ! getline(3, resp, '\0')) {
			LOG( ::vq::ilogger::re_read, strerror(errno));
			return(1);
	}
	login = lower(login);
	string::size_type atpos = login.find_last_of(atchars);
	string dom;
	if( atpos != login.npos ) {
			dom = login.substr(atpos+1);
			login = login.substr(0,atpos);
	}
	if( login.empty() ) {
			LOG(::vq::ilogger::re_data, "empty login" );
			return(1);
	}
	vqai.login = login.c_str();
	olog->login_set(vqai.login);
	
	if( dom.empty() ) dom = defdom ? defdom : locip;
	if( dom.empty() ) {
			LOG(::vq::ilogger::re_data, "empty domain");
			return 1;
	}
	olog->domain_set(dom.c_str());

	ret=ovq->dom_id(dom.c_str(), vqai.id_domain);
	if( ret->ec != ::vq::ivq::err_no ) {
			LOG(::vq::ilogger::re_data, error2str(ret).c_str() );
			return 1;
	}

	if( pass.empty() ) {
			LOG(::vq::ilogger::re_data, "empty password");
			return(1);
	}
	return 0;
}

/*
 *
 */
char read_env() {
	const char *tmp;
	int itmp;
	while((itmp=getopt(ac, av, "@:D:")) != -1) {
			switch(itmp) {
			case 'D':
					defdom = optarg;
					break;
			case '@':
					atchars = optarg;
					break;
			}
	}
	ac -= optind;
	av += optind;

	remip = getenv("TCPREMOTEIP");
	if(!remip) remip = "";
	
	locip = getenv("TCPLOCALHOST");
	if( !locip || *locip == '\0' ) {
			locip = getenv("TCPLOCALIP");
			if(!locip || *locip == '\0') {
					locip = "127.0.0.1";
			}
	}			
	if( (tmp=getenv("TCPLOCALPORT")) ) {
			istringstream is;
			is.str(tmp);
			is >> itmp;
			if( ! is || itmp<0 || itmp>0xffff ) {
					LOG( ::vq::ilogger::re_int, 
						"TCPLOCALPORT is not a valid port number");
					return(1);
			}
	} else {
			itmp = -1;
	}

	if( (tmp=getenv("VQ_ATCHARS")) ) {
			atchars = tmp;
	}

	if( atchars.empty() ) {
			LOG( ::vq::ilogger::re_int, "atchars empty");
			return 1;
	}

	switch(itmp) {
	case 995:
	case 110:
			if(ac<1) {
					LOG( ::vq::ilogger::re_int, 
						"no command was given");
					return(1);
			}
			contype = pop3;
			olog->service_set(::vq::ilogger::ser_pop3);
			break;
	case 80: // HTTP
	case 443: // HTTPS
	case 465: // ESMTP+SSL
	case 25: // ESMTP
	case 250: // EMTP
			contype = smtp;

			switch(itmp) {
			case 80:
					olog->service_set(::vq::ilogger::ser_http);
					break;
			case 443:
					olog->service_set(::vq::ilogger::ser_https);
					break;
			case 250:
					olog->service_set(::vq::ilogger::ser_emtp);
					break;
			default:
					olog->service_set(::vq::ilogger::ser_unknown);
			}
			break;
	default:
			olog->service_set(::vq::ilogger::ser_unknown);
			contype = ac > 1 ? pop3 : smtp;
	}
	return 0;
}

/*
 * return 0 if apop authorization is successful
 */
char auth_apop() {
	MD5_CTX ctx;
	unsigned char digest[16];
	string hexdigest;
	MD5Init(&ctx);
	MD5Update(&ctx,
		reinterpret_cast<const unsigned char*>(resp.data()),
		resp.length());
	MD5Update(&ctx,
		reinterpret_cast<const unsigned char*>(&*vqai.pass),
		vqai.pass ? strlen(vqai.pass) : 0);
	MD5Final(digest,&ctx);
	hexdigest = to_hex(digest,16);
	return pass==hexdigest ? 0 : 1;
}
/*
 * return 0 if cram-md5 authorization is successful
 */
char auth_cram() {
	char digest[16];
	hmac_md5( vqai.pass ? "" : vqai.pass, vqai.pass ? strlen(vqai.pass) : 0, 
		resp.data(), resp.length(), digest);
	return to_hex(digest, sizeof digest) == pass ? 0 : 1;
}

/*
 *
 */
char login_smtp() {
	LOG( ::vq::ilogger::re_ok, "" );
	return(0);
}

/*
 *
 */
char login_pop() {
	if( setenv("HOME", vqai.dir ? vqai.dir : "", 1) ) {
			return(1);
	}
	if( chdir(vqai.dir) ) {
			return(1);
	}
	if( setgid(vqai.gid) || setegid(getgid()) 
		|| setuid(vqai.uid) || seteuid(getuid()) ) {
			return(1);
	}
	LOG(::vq::ilogger::re_ok, "" );

	olog = NULL;
	ovq = NULL;
	execvp(*av, av);
	return(1);
} 

/*
 *
 */
char proc_auth_info() {
	if(contype==smtp) {
			if( vqai.flags & vq::ivq::uif_smtp_blk ) {
					LOG(::vq::ilogger::re_blk, "" );
					return(1);
			}
			if( (! resp.empty() && ! auth_cram())
				|| pass==static_cast<char *>(vqai.pass) ) {
					return login_smtp();
			}
	} else {
			if( vqai.flags & vq::ivq::uif_pop3_blk ) {
					LOG(::vq::ilogger::re_blk, "" );
					return(1);
			}
			if( (! resp.empty() && ! auth_apop())
				|| pass==static_cast<const char *>(vqai.pass) ) {
					return login_pop();
			}
	}
	LOG( ::vq::ilogger::re_pass, pass.c_str() );
	return(1);
}

/*
 *
 */
int cluemain(int ac, char **av, cluemain_env & ce ) try {
	::av = av;
	::ac = ac;
	
	conf::clnconf ilogger_import(VQ_HOME+"/etc/ilogger/ilogger_import", "name_service#Logger.ilogger");

	CORBA::Object_var ilogobj;
	try {
			ilogobj = corbautil::importObjRef(ce.orb, 
				ilogger_import.val_str().c_str());
	} catch( corbautil::ImportExportException & e ) {
			throw;
	}
	
	olog = ::vq::ilogger::_narrow(ilogobj);
	ovq = ce.vq;

	try {
			if(read_env()) return 3;
			
			olog->ip_set(remip);

			if(read_auth_info()) return 2;

			ret = ovq->user_get(vqai);
			if( ::vq::ivq::err_no != ret->ec ) {
					LOG( ret->ec == ::vq::ivq::err_noent 
						? ::vq::ilogger::re_data : ::vq::ilogger::re_int, 
						error2str(ret).c_str() );
			}

			return proc_auth_info();
	} catch( exception & e ) {
			LOG( ::vq::ilogger::re_int, e.what());
			return 1;
	} catch( vq::except & e ) {
			std::ostringstream os;
			os<<"Exception: "<<e.what<<" in "<<e.file<<" at "<<e.line;
			LOG( ::vq::ilogger::re_int, os.str().c_str());
			return 111;
	} catch( vq::db_error & e ) {
			std::ostringstream os;
			os<<"Database exception: "<<e.what<<" in "<<e.file<<" at "<<e.line;
			LOG( ::vq::ilogger::re_int, os.str().c_str());
			return 111;
	} catch( vq::null_error & e ) {
			std::ostringstream os;
			os<<"Unexpected NULL value in "<<e.file<<" at "<<e.line;
			LOG( ::vq::ilogger::re_int, os.str().c_str());
			return 111;
	} catch( CORBA::SystemException & e ) {
			std::ostringstream os;
			os<<"CORBA system exception: ";
#if defined(MICO_VERSION)
			e._print(os);
#elif defined(OMNIORB_DIST_DATE)
			os<<"minor: "<<e.NP_minorString()<<"; completed: ";
			switch(e.completed()) {
			case CORBA::COMPLETED_YES: os<<"YES"; break;
			case CORBA::COMPLETED_NO: os<<"NO"; break;
			default: os<<"MAYBE";
			}
#else
			os<<"minor: "<<e.minor()<<"; completed: ";
			switch(e.completed()) {
			case CORBA::COMPLETED_YES: os<<"YES"; break;
			case CORBA::COMPLETED_NO: os<<"NO"; break;
			default: os<<"MAYBE";
			}
#endif
			LOG( ::vq::ilogger::re_int, os.str().c_str());
			return 111;
	} catch( CORBA::Exception & e ) {
			std::ostringstream os;
			os<<"CORBA exception";
#ifdef MICO_VERSION	
			os<<": ";
			e._print(os);
#endif
			LOG( ::vq::ilogger::re_int, os.str().c_str());
			return 111;
	} catch( ... ) {
			LOG( ::vq::ilogger::re_int, "exception" );
			return 1;
	}
	return 0;
} catch( ... ) {
	return 111;
}

int vqmain( int ac, char ** av ) try {
	using namespace std;

	conf::clnconf ivq_import(VQ_HOME+"/etc/ivq/ivq_import", "name_service#VQ.ivq");
	/*
	 * Initialize the ORB
	 */
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
			//cerr<<e<<endl;
			ce.orb->destroy();
			return 100;
	}

	try {
			ce.vq = vq::ivq::_narrow(ivqobj);
			if( CORBA::is_nil(ce.vq) ) {
					//cerr<<"can't narrow "<<ivq_import.val_str()<<endl;
					return 100;
			}
	} catch(...) {
			cerr<<"Exception while resolving ivq implementation"<<endl;
			//throw;
			ce.orb->destroy();
			return 100;
	}	
	
	return cluemain(ac, av, ce);
} catch(...) {
	return 100;
}
