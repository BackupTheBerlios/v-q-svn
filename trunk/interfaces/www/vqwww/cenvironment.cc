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
#include <stdexcept>

#include "cenvironment.h"
#include "conf_home.h"

using namespace std;
using namespace vqwww;

bool cenvironment::instance = false;

cenvironment::cenvironment( int ac, const char ** av,
		std::ostream & o, std::ostream & e, xslt_args_type & xat ) 
		: ac(ac), av(av), cgi(new cgicc::Cgicc()),
		dirs(conf_home), err(e), out(o), xslt_args(xat) {

	if( instance ) 
			throw logic_error("cenvironment has already an instance!!!");
	instance = true;
			
	vq.reset(cvq::alloc());
	sess.reset(csess::alloc());
	logger.reset(clogger::alloc());
	cgi_rm = cgicc::stringsAreEqual(cgi_env_get().getRequestMethod(), "post") ? rm_post : rm_get;
}
