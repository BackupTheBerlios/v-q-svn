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

#ifndef CMAILDIR_H
#define CMAILDIR_H

#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <deque>

class cmaildir {
        protected:
                /// error message
                std::string err;
                /// maildir's name
                std::string name;
                /// is maildir opened ?
                bool isopen;
                DIR * dcur, * dnew, * dtmp;
        public:
                cmaildir() : isopen(false) {}
                bool open( const std::string & );
                bool close();
                bool create( const std::string &, bool = false );
                const std::string & errmsg() { return err; }
                bool rm();
                bool lsdir( const std::string &, std::deque<std::string> &, bool = false );
                
                enum sub_dir { cur, tmp, new };
};

#endif
