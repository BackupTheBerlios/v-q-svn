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
