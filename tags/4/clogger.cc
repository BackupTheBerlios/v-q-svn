/*
Copyright (c) 2002,2003 Pawel Niewiadomski
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the Pawel Niewiadomski,
   and other contributors.
4. Neither the name of Pawel Niewiadomski nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY PAWEL NIEWIADOMSKI AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
iMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL PAWEL NIEWIADOMSKI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/
#include <cerrno>

#include "clogger.h"
#include "lower.h"

using namespace std;

/**
 *
 */
clogger::clogger() : ser(ser_unknown), lr_errno(0), lastret(err_no), 
		lastret_blkd(false) {
}

/**
 * 
 */
clogger::~clogger() {
}

/**
 *
 */
void clogger::ip_set(const std::string &_ip) {
	ip = _ip;
}

/**
 *
 */
void clogger::service_set( service s ) {
	ser = s;
}

/**
 * 
 */
void clogger::domain_set( const std::string & d ) {
	dom = lower(d);	
}

/**
 *
 */
void clogger::login_set( const std::string & u ) {
	log = lower(u);
}

/**
 *
 */
uint8_t clogger::lr( uint8_t ret, const std::string & msg ) { 
		if( ret != err_no && ret != lastret && ! lastret_blkd ) {
				lastret = ret;
				lr_info = msg;
				lr_errno = errno;
		}
		return ret;
}

/**
 *
 */
string clogger::err_str( uint8_t e ) {
	return err_str( static_cast<error>(e) );
}

/**
 *
 */
string clogger::err_str( error e ) {
	switch(e) {
	case err_no:
			return "Success";
	case err_temp:
			return "Unknown error";
	case err_dom_log:
			return "There's no log table for specified domain";
	case err_empty:
			return "Result is empty";
	case err_rd:
			return "Read error";
	case err_wr:
			return "Write error";
	default:
			return "Unknown error";
	}
}

/**
 *
 */
string clogger::err_info() const {
	return lr_info.c_str();
}

/**
 *
 */
int clogger::err_sys() const {
	return lr_errno;
}

/**
 *
 */
string clogger::err_report() const {
	return err_str(lastret)+": "+lr_info+": "+strerror(lr_errno);
}
