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

#ifndef CSESS_H
#define CSESS_H

#include <string>
#include <ctime>

/**
 * \defgroup sess Sessions handling
 */
//@{

/**
 * Abstract class representing sessions.
 * \author Pawel Niewiadomski
 * \version 1
 */
class csess {
        public:
                virtual ~csess() {}
                /// returns id of current session
                virtual std::string getid()=0;
                /// check whether session with given id is valid
                virtual bool valid( const std::string & ) = 0;
                /// store time
                virtual bool setval( const std::string &, const struct timeval & )=0;
                /// read time
                virtual bool getval( const std::string &, struct timeval & )=0;
                /// store value within session, if value exists it's replaced, returns false on error
                virtual bool setval( const std::string &, const std::string & ) = 0;
                /// store bool value
                virtual bool setval( const std::string &, bool ) = 0;
                /// store binary value
                virtual bool setval( const std::string &, const char *, unsigned ) = 0;
				//@{
                /// get binary value
                virtual bool getval( const std::string &, char *, unsigned ) = 0;
                virtual bool getval( const std::string &, char **, unsigned & ) = 0;
				//@}
                /// store char *
                virtual bool setval( const char *, const char * ) = 0;
                /// get bool value
                virtual bool getval( const std::string &, bool & ) = 0;
                /// gets value from session, if value isn't present returns "" and false, returns false on error
                virtual bool getval( const std::string &, std::string & ) = 0;
                /// remove value from session, returns false on error
                virtual bool rmval( const std::string & ) = 0;
                /// remove session, mark session closed, returns false on error
                virtual bool rm() = 0;
                /// open session, returns false on error
                virtual bool open( const std::string & ) = 0;
                /// create and open session, returns false on error
                virtual bool create( std::string & ) = 0;
                /// return error message
                virtual const char * errmsg() = 0;
                /// get session's directory
                virtual const char * dir() = 0;

				/// alloc implementation object
				static csess *alloc();
};
//@}

#endif // ifndef CSESS_H
