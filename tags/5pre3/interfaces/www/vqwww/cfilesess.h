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

#ifndef CFILESESS_H
#define CFILESESS_H

#include "csess.h"

/**
 * \author Pawel Niewiadomski
 * \see csess
 */

/**
 * Class for storing sessions on disk.
 * Each session is stored as a directory containing files - session's values. Names of values are converted to hex.
 */
class cfilesess : public csess {
        protected:
                std::string base, err, id, sid;

				//@{
				/// convert to hexadecimal
                std::string tohex( const std::string & );
                std::string tohex( const char *, unsigned );
				//@}
                std::string crtid(); //!< create session id.
                std::string sessfn(const std::string &); //!< creates path for given session id

        public:
                cfilesess( const std::string & );
                ~cfilesess() {}
                std::string getid() { return sid; }
                bool valid( const std::string & );
                bool setval( const std::string &, const struct timeval & );
                bool getval( const std::string &, struct timeval &);
                bool setval( const std::string &, const std::string & );
                bool setval( const std::string &, const char *, unsigned );
                bool getval( const std::string &, char *, unsigned );
                bool getval( const std::string &, char **, unsigned & );
                bool getval( const std::string &, std::string & );
                bool setval( const std::string &, bool );
                bool getval( const std::string &, bool & );
                bool rmval( const std::string & );
                bool setval( const char *n, const char * str ) 
                        { return setval((std::string)n, (std::string) str); }
                bool rm();
                bool open( const std::string & );
                bool create( std::string & );
                const char *errmsg();
                const char *dir() {
                        return id.c_str();
                }
};

#endif
