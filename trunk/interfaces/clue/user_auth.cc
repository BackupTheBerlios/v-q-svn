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

#include <sys/types.h>
#include <unistd.h>

#include <exception>
#include <iostream>
#include <cerrno>
#include <string>
#include <sstream>

using namespace std;
using namespace vq;
using namespace text;
using namespace sys;

char *me = NULL, **av;
int ac;
const char *remip = NULL, *locip = NULL, *defdom = NULL;
bool quiet = false;
cvq *vqo = NULL;
string resp, pass;
enum { smtp, pop3 } contype;
cvq::auth_info vqai;
clogger *olog = NULL;

/*
 *
 */
char read_auth_info() {
	if( ! getline(3,vqai.user,'\0')
		|| ! getline(3,pass,'\0')
		|| ! getline(3,resp,'\0')) {
			olog->write( clogger::re_read, strerror(errno));
			return(1);
	}
	vqai.user = lower(vqai.user);
	if( ac_atchars.val_str().empty() ) {
			olog->write( clogger::re_int, "ac_atchars empty");
			return(1);
	}
	string::size_type atpos = vqai.user.find_last_of(ac_atchars.val_str());
	if( atpos != vqai.user.npos ) {
			vqai.dom = vqai.user.substr(atpos+1);
			vqai.user = vqai.user.substr(0,atpos);
	}

	if( vqai.dom.empty() ) vqai.dom = defdom ? defdom : locip;
	
	if( vqai.dom.empty() || !vqo->dom_val(vqai.dom) ) {
			olog->write(clogger::re_data, 
				(string)"invalid domain name: "+vqai.dom);
			return 1;
	}
	
	olog->domain_set(vqai.dom);

	if( vqai.user.empty() || !vqo->user_val(vqai.user) ) {
			olog->write(clogger::re_data, 
				(string)"invalid user name: "+vqai.user );
			return(1);
	}
	
	olog->login_set(vqai.user);
	
	if( pass.empty() ) {
			olog->write(clogger::re_data, "empty password");
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
	if( !(tmp=getenv("TCPLOCALPORT")) ) {
			cerr<<me<<": TCPLOCALPORT is not set"<<endl;
			return(1);
	} else {
			istringstream is;
			is.str(tmp);
			is >> itmp;
			if( ! is || itmp<0 || itmp>0xffff ) {
					cerr<<me<<": TCPLOCALPORT is not a valid port number"<<endl;
					return(1);
			}
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
			olog->service_set(clogger::ser_pop3);
			break;
	case 80: // HTTP
	case 443: // HTTPS
	case 465: // ESMTP+SSL
	case 25: // ESMTP
	case 250: // EMTP
			contype = smtp;

			switch(itmp) {
			case 80:
					olog->service_set(clogger::ser_http);
					break;
			case 443:
					olog->service_set(clogger::ser_https);
					break;
			case 250:
					olog->service_set(clogger::ser_emtp);
					break;
			default:
					olog->service_set(clogger::ser_smtp);
			}
			break;
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
	olog->write( clogger::re_ok, "" );
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
	olog->write(clogger::re_ok, "" );

	delete olog; olog = NULL;
	delete vqo; vqo = NULL;
	execvp(*av,av);
	return(1);
} 

/*
 *
 */
char proc_auth_info() {
	if(contype==smtp) {
			if( vqai.flags & cvq::smtp_blk ) {
					olog->write(clogger::re_blk, "" );
					return(1);
			}
			if( (! resp.empty() && ! auth_cram())
				|| pass==vqai.pass ) {
					return login_smtp();
			}
	} else {
			if( vqai.flags & cvq::pop3_blk ) {
					olog->write(clogger::re_blk, "" );
					return(1);
			}
			if( (! resp.empty() && ! auth_apop())
				|| pass==vqai.pass ) {
					return login_pop();
			}
	}
	olog->write( clogger::re_pass, pass );
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
	//ce.vq = vq::ilogger::_narrow(ivqobj);
	//if( CORBA::is_nil(ce.vq) ) {
	//		return 1;
	//}

	sig_pipe_ign();

	try {
			if(read_env()) return 3;
			
			olog->ip_set(remip);

			if(read_auth_info()) return 2;

			char ret;
			switch( (ret=vqo->user_auth(vqai)) ) {
			case 0: break;
			case cvq::err_user_nf:
					olog->write( clogger::re_data, "no such user" );
					return(1);
			default:
					olog->write( clogger::re_int, vqo->err_report());
					return 1;
			}

			return proc_auth_info();
	} catch( ... ) {
			olog->...
			return(1);
	}
	return 0;
} catch( ... ) {
	return 1;
}
