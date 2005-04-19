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
#ifndef __CPATHS_HPP
#define __CPATHS_HPP

#include <text.hpp>
#include <split.hpp>

#include <string>

class cpaths {
		public:
				cpaths( const std::string & d, unsigned s_dom, unsigned s_user ) 
					: domains(d), dom_split(s_dom), user_split(s_user) {}

				/**
				 * \defgroup path Paths handling
				 */
				//@{
					inline std::string dom_path( const std::string & ) const;

					inline std::string user_root_path(const std::string &, 
							const std::string &) const;
					inline std::string user_root_subpath(const std::string &) const;
					
					inline std::string user_dir_path(const std::string &, 
							const std::string &) const;

					inline std::string user_md_path(const std::string &, 
							const std::string &) const;
					inline std::string user_md_subpath(const std::string &) const;
				//@}

		protected:
				std::string domains;
				unsigned dom_split;
				unsigned user_split;
}; // cpath
	
/**
 * \return Path for domain that will be put into assign
 * \param dom_id domain's id.
 */
std::string cpaths::dom_path( const std::string & dom_id ) const {
	return this->domains + '/' + text::split_id(dom_id, this->dom_split) 
		+ '/' + dom_id;
}

/**
 *
 */
std::string cpaths::user_root_subpath( const std::string & login ) const {
	return text::split_user(text::lower(login), this->user_split);
}

/**
 * \return Path to directory where user's .qmail files are located
 */
std::string cpaths::user_root_path( const std::string &dom_id,
		const std::string & login ) const {
	return dom_path(dom_id) + '/' + user_root_subpath(login);
}

/**
 * \return Path to user's directory
 */
std::string cpaths::user_dir_path( const std::string &dom_id,
		const std::string & login ) const {
	return user_root_path(dom_id, text::lower(login)) + '/' + login;
}

/**
 * \return Path to user's maildir
 */
std::string cpaths::user_md_path( const std::string &dom_id,
		const std::string & login ) const {
	return user_dir_path(dom_id, login) + "/Maildir/";
}

/**
 *
 */
std::string cpaths::user_md_subpath( const std::string & login ) const {
	return "./"+login+"/Maildir/";
}

#endif
