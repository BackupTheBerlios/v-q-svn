/*
Copyright (c) 2002,2003 Pawel Niewiadomski
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the Pawel Niewiadomski,
   and other contributors.
4. Neither the name of Pawel Niewiadomski nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY PAWEL NIEWIADOMSKI AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL PAWEL NIEWIADOMSKI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
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
