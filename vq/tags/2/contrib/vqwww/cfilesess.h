#ifndef CFILESESS_H
#define CFILESESS_H

#include "csess.h"

/*
Copyright (c) 2002 Pawel Niewiadomski
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

/**
@author Pawel Niewiadomski
@version 1
@see csess
@memo Class for storing sessions on disk.
@doc Each session is stored as a directory containing files - session's values. Names of values are converted to hex.
*/
class cfilesess : public csess {
        protected:
                std::string base, err, id, sid;

                std::string tohex( const std::string & );
                std::string tohex( const char *, unsigned );
                std::string crtid();
                std::string sessfn(const std::string &); //!< creates path for given session id

                static const int conf_sess_fmode_i = 0600;
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
