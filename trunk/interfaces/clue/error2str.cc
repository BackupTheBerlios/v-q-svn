/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new@foo-baz.com)

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
#include "error2str.hpp"

#include <sstream>

std::string error2str( vq::ivq::error_var & err ) {
	std::ostringstream os;
	os<<"#"<<err->ec<<": ";
	switch( err->ec ) {
	case ::vq::ivq::err_no: os<<"success"; break;
	case ::vq::ivq::err_temp: os<<"unknown temporary error"; break;
	case ::vq::ivq::err_dom_inv: os<<"invalid domain"; break;
	case ::vq::ivq::err_user_inv: os<<"invalid user"; break;
	case ::vq::ivq::err_auth: os<<"unknown problem with authorization module"; break;
	case ::vq::ivq::err_user_conf_tns: os<<"type not supported in user's configuration"; break;
	case ::vq::ivq::err_open: os<<"open failed"; break;
	case ::vq::ivq::err_wr: os<<"write failed"; break;
	case ::vq::ivq::err_rd: os<<"read failed"; break;
	case ::vq::ivq::err_stat: os<<"stat failed"; break;
	case ::vq::ivq::err_notdir: os<<"directory required"; break;
	case ::vq::ivq::err_chmod: os<<"chmod failed"; break;
	case ::vq::ivq::err_lckd: os<<"locked"; break;
	case ::vq::ivq::err_ren: os<<"rename failed"; break;
	case ::vq::ivq::err_chown: os<<"chown failed"; break;
	case ::vq::ivq::err_over: os<<"over"; break;
	case ::vq::ivq::err_exec: os<<"exec failed"; break;
	case ::vq::ivq::err_mkdir: os<<"mkdir failed"; break;
	case ::vq::ivq::err_exists: os<<"exists"; break;
	case ::vq::ivq::err_noent: os<<"missing element"; break;
	case ::vq::ivq::err_unlink: os<<"unlink failed"; break;
	case ::vq::ivq::err_pass_inv: os<<"invalid password"; break;
	case ::vq::ivq::err_func_res: os<<"unsupported return value"; break;
	case ::vq::ivq::err_func_ni: os<<"function not implemented"; break;
	}

	os<<" in "<<err->file<<" at "<<err->line;
	if( '\0' != *err->what ) {
			os<<": "<<err->what;
	}
	return os.str();
}

