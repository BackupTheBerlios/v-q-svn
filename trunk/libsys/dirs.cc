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
#include "dirs.hpp"

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include <string>
#include <cerrno>

namespace sys {

	using namespace std;
	
	/**
	 * create directories hierarchy
	 * \return true on success
	 */
	bool mkdirhier(const char *path, mode_t m)
	{
		if( mkdir(path,m) ) {
				if( errno != ENOENT ) return false;
				string pre, sub, p(path);
				string::size_type pos, beg;
				for( beg=0; (pos=p.find('/',beg)) != string::npos; beg = pos+1 ) {
						sub = p.substr(beg, pos-beg);
						if( sub == ".." ) return false;
						if( sub == "." ) continue;
						pre += sub + '/';
						if( sub == "" ) continue;
						if( mkdir( pre.c_str(), m ) && errno != EEXIST ) {
								return false;
						}
				}
				pre += p.substr(beg);
				if( mkdir(pre.c_str(), m) ) return false;
				return true;
		}
		return true;
	}
	
	/**
	 * create directories hierarchy, change owners for new directories
	 * \return true on success
	 */
	bool mkdirhier(const char *path, mode_t m, int uid, int gid)
	{
		if( mkdir(path,m) ) {
				if( errno != ENOENT ) return false;
				string pre, sub, p(path);
				string::size_type pos, beg;
				for( beg=0; (pos=p.find('/',beg)) != string::npos; beg = pos+1 ) {
						sub = p.substr(beg, pos-beg);
						if( sub == ".." ) return false;
						if( sub == "." ) continue;
						pre += sub + '/';
						if( sub == "" ) continue;
						if( mkdir( pre.c_str(), m ) ) { 
								if( errno != EEXIST ) return false;
						} else if( chown(pre.c_str(), uid, gid) ) return false;
				}
				pre += p.substr(beg);
				if( mkdir(pre.c_str(), m)
					|| chown(pre.c_str(), uid, gid)) return false;
				return true;
		}
		if( chown(path, uid, gid) ) return false;
		return true;
	}
	
	/**
	 * \exception runtime_error error occured, application possibly can't continue
	 * \return false on error which isn't critical
	 */
	bool rmdirrec( const string & n )
	{
			DIR *d = opendir(n.c_str());
			if( ! d ) {
					return false;
			}
			struct dirent *de;
			struct stat st;
			std::string fn;
			while( (de=readdir(d)) ) {
					if( de->d_name[0] == '.' && ( de->d_name[1] == '\0' 
						|| (de->d_name[1] == '.' && de->d_name[2] == '\0' )) )
							continue;

					fn = n + '/' + de->d_name;
							
					if( ::remove( fn.c_str() ) ) {
							if( stat( fn.c_str(), & st) ) {
									return false;
							}
							if( S_ISDIR(st.st_mode) ) {
									if( ! rmdirrec( fn ) )
											return false;
							} else {
									return false;
							}
					}
			}
			return remove(n.c_str()) ? false : true;
	}

} // namespace sys
