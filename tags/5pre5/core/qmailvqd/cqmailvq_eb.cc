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
#include "cqmailvq.hpp"
#include "cqmailvq_common.hpp"

namespace POA_vq {

	/**
	 *
	 */
	cqmailvq::error* cqmailvq::eb_add( const char* re_domain, 
			const char* re_login ) std_try {
	
		if( ! re_domain || ! re_login )
				throw ::vq::null_error(__FILE__, __LINE__);
		return auth->eb_add(re_domain, re_login);
	} std_catch
	
	/**
	 *
	 */
	cqmailvq::error* cqmailvq::eb_rm( const char* re_domain, 
			const char* re_login ) std_try {
		if( ! re_domain || ! re_login )
				throw ::vq::null_error(__FILE__, __LINE__);
		return auth->eb_rm(re_domain, re_login);
	} std_catch
	
	/**
	 *
	 */
	cqmailvq::error* cqmailvq::eb_ls( email_banned_list_out ebs ) std_try {
		return auth->eb_ls(ebs);
	} std_catch
	
} // namespace POA_vq	
