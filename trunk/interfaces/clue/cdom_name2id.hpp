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
#ifndef __DOM_NAME2ID_HPP
#define __DOM_NAME2ID_HPP

#include <vq.hpp>

#include <string>

namespace clue {

	class cdom_name2id {
			public:
					inline ::vq::ivq::error_var operator()( ::vq::ivq_var &,
							const std::string &, CORBA::String_var & );

			protected:
					typedef std::map< std::string, CORBA::String_var > domain2id_map;
					static domain2id_map dom2id;
	};

	inline ::vq::ivq::error_var cdom_name2id::operator()( ::vq::ivq_var &vq,
			const std::string & dom, CORBA::String_var & id ) {

		domain2id_map::const_iterator did_itr( this->dom2id.find(dom) );
		::vq::ivq::error_var ret(new ::vq::ivq::error);
		if( this->dom2id.end() == did_itr ) {
				CORBA::String_var did;
				ret = vq->dom_id(dom.c_str(), did);
				if( ::vq::ivq::err_no != ret->ec ) {
						return ret;
				}
				did_itr = dom2id.insert( dom2id.begin(), 
						std::make_pair(dom, did) );
		}
		id = did_itr->second;
		ret->ec = ::vq::ivq::err_no;
		return ret;
	}

} // namespace clue
#endif // ifndef __DOM_NAME2ID_HPP
