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
#include "hmac_md5.hpp"
#include "auto/lmd5.h"
#include "cluemain.hpp"
#include "error2str.hpp"

#include <logger.hpp>
#include <text.hpp>
#include <sys.hpp>
#include <conf.hpp>
#include <vqmain.hpp>

#include <iostream>
#include <sstream>

using namespace std;
using namespace vq;
using namespace text;
using namespace sys;
using clue::error2str;

char *me = NULL, **av;
int ac;
const char *remip = NULL, *locip = NULL, *defdom = NULL;
bool quiet = false;
string resp, pass, login;
enum { smtp, pop3 } contype;

::vq::ilogger_var olog;
::vq::ivq_var ovq;

::vq::ivq::auth_info vqai;
::vq::ivq::error_var ret;

#define LOG(x,y) delete olog->write(x,y)

/*
 *
 */
char read_auth_info() {
	if( ! getline(3,login,'\0')
		|| ! getline(3,pass,'\0')
		|| ! getline(3,resp,'\0')) {
			LOG( ::vq::ilogger::re_read, strerror(errno));
			return(1);
	}
	login = lower(login);
	if( ac_atchars.val_str().empty() ) {
			LOG( ::vq::ilogger::re_int, "ac_atchars empty");
			return(1);
	}
	string::size_type atpos = login.find_last_of(ac_atchars.val_str());
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
	if( dom.empty() )
			LOG(::vq::ilogger::re_data, "empty domain");
			return 1;
	}
	olog->domain_set(dom);

	ret=ovq->dom_id(dom.c_str(), vqai.id_domain);
	if( ret->ec != ::vq::ivq::err_no ) {
			LOG(::vq:ilogger::re_data, error2str(ret));
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
	while((itmp=getopt(ac, av, "D:")) != -1) {
			switch(itmp) {
			case 'D':
					defdom = optarg;
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
	if( (tmp=getenv("TCPLOCALPORT")) ) 
			istringstream is;
			is.str(tmp);
			is >> itmp;
			if( ! is || itmp<0 || itmp>0xffff ) {
					cerr<<me<<": TCPLOCALPORT is not a valid port number"<<endl;
					return(1);
			}
	} else {
			itmp = -1;
	}

	switch(itmp) {
	default:
	case 995:
	case 110:
			if(ac<1) {
					cerr<<me<<": no command was given"<<endl;
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
	MD5Update(&ctx,(const unsigned char*)resp.data(),resp.length());
	MD5Update(&ctx,(const unsigned char*)vqai.pass.data(),vqai.pass.length());
	MD5Final(digest,&ctx);
	hexdigest = to_hex(digest,16);
	return pass==hexdigest ? 0 : 1;
}
/*
 * return 0 if cram-md5 authorization is successful
 */
char auth_cram() {
	unsigned char digest[16];
	string hexdigest;
	hmac_md5((const unsigned char*)vqai.pass.data(),
					vqai.pass.length(),(const unsigned char*)resp.data(),
					resp.length(),digest);
	hexdigest = to_hex(digest,16);
	return pass==hexdigest ? 0 : 1;
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
	if( setenv("HOME", vqai.dir.c_str(), 1) ) {
			return(1);
	}
	if( chdir(vqai.dir.c_str()) ) {
			return(1);
	}
	if( setgid(ac_vq_gid.val_int()) || setegid(getgid()) 
		|| setuid(ac_vq_uid.val_int()) || seteuid(getuid()) ) {
			return(1);
	}
	LOG(::vq::ilogger::re_ok, "" );

	delete olog; olog = NULL;
	delete ovq; ovq = NULL;
	execvp(*av,av);
	return(1);
} 

/*
 *
 */
char proc_auth_info() {
	if(contype==smtp) {
			if( vqai.flags & cvq::smtp_blk ) {
					LOG(::vq::ilogger::re_blk, "" );
					return(1);
			}
			if( (! resp.empty() && ! auth_cram())
				|| pass==vqai.pass ) {
					return login_smtp();
			}
	} else {
			if( vqai.flags & cvq::pop3_blk ) {
					LOG(::vq::ilogger::re_blk, "" );
					return(1);
			}
			if( (! resp.empty() && ! auth_apop())
				|| pass==vqai.pass ) {
					return login_pop();
			}
	}
	LOG( ::vq::ilogger::re_pass, pass );
	return(1);
}

/*
 *
 */
int cluemain(int ac, char **av, cluemain_env & ce ) try {
	me = *av;
	::av = av;
	::ac = ac;
	
	conf::clnconf ilogger_name(VQ_HOME+"/etc/ilogger/ilogger_name", "vq::ilogger");
	/*
	 * Get iauth object
	 */
	CosNaming::Name obj_name;
	obj_name.length(1);
	obj_name[0].id = ilogger_name.val_str().c_str();
	obj_name[0].kind = static_cast<const char *>("");

	CORBA::Object_var ilogobj;
	vq::ilogger_var log;
	ilogobj = ce.ns->resolve(obj_name);
	if( CORBA::is_nil( ilogobj ) ) {
			return 1;
	}
	olog = ::vq::ilogger::_narrow(ilogobj);
	ovq = ce.vq;

	sig_pipe_ign();

	try {
			if(read_env()) return 3;
			
			olog->ip_set(remip);

			if(read_auth_info()) return 2;

			ret = ovq->user_get(vqai);
			if( ::vq::ivq::err_no != ret->ec ) {
					LOG( ret->ec == ::vq::ivq::err_noent 
						? ::vq::ilogger::re_data : ::vq::ilogger::re_int, 
						error2str(ret) );
			}

			return proc_auth_info();
	} catch( ... ) {
			LOG( ::vq::ilogger::re_int, "exception" );
			return(1);
	}
	return 0;
} catch( ... ) {
	return 1;
}
