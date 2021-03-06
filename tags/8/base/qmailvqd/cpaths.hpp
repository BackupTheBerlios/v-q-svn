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

namespace vq {
	class cpaths {
			public:
					typedef unsigned size_type;
					
					cpaths( const std::string & d, 
						size_type s_dom, size_type s_user,
						const std::string & md ) 
						: domains(d), dom_split(s_dom), user_split(s_user),
						maildir(md) {}
	
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

						inline size_type dom_split_get() const;
						inline size_type user_split_get() const;

						inline std::string maildir_get() const;
					//@}
	
			protected:
					std::string domains;
					size_type dom_split;
					size_type user_split;
					std::string maildir;
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
		return user_dir_path(dom_id, login) + "/" + this->maildir + "/";
	}
	
	/**
	 *
	 */
	std::string cpaths::user_md_subpath( const std::string & login ) const {
		return "./"+login+"/" + this->maildir + "/";
	}
	
	cpaths::size_type cpaths::dom_split_get() const {
		return this->dom_split;
	}
	
	cpaths::size_type cpaths::user_split_get() const {
		return this->user_split;
	}

	std::string cpaths::maildir_get() const {
		return this->maildir;
	}

} // namespace vq

#endif
