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
iMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL PAWEL NIEWIADOMSKI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

#ifndef __CLOG_H
#define __CLOG_H

#include <string>

/**
 * Base class for logging subsystem.
 */
class clogger {
		public:
				/// error message
				virtual void e(const std::string &) = 0;
				/// error message, function ads strerror(errno) message at end of std::string
				virtual void estr(const std::string &) = 0;
				/// warning
				virtual void w(const std::string &) = 0;
				/// warning, with strerror(errno) appended
				virtual void wstr(const std::string &) = 0;
				/// notice, info
				virtual void n(const std::string &) = 0;
				/// 
				virtual void nstr(const std::string &) = 0;
				/// debug info
				virtual void d(const std::string &) = 0;
				/// debug info, with strerror(errno) appended
				virtual void dstr(const std::string &) = 0;
				
				/// authorization error
				virtual void ae(const std::string &,const std::string & = "",
								const std::string & = "") = 0;
				/// authorization error, with strerror(errno) appended
				virtual void aestr(const std::string &,const std::string & = "",
								const std::string & = "") = 0;
				/// authorization warning
				virtual void aw(const std::string &,const std::string & = "",
								const std::string & = "") = 0;
				/// authorization warning, with strerror(errno) appended
				virtual void awstr(const std::string &,const std::string & = "",
								const std::string & = "") = 0;
				/// authorization notice
				virtual void an(const std::string &,const std::string & = "",
								const std::string & = "") = 0;
				/// authorization notice , with strerror(errno) appended
				virtual void anstr(const std::string &,const std::string & = "",
								const std::string & = "") = 0;
				/// authorization debug
				virtual void ad(const std::string &,const std::string & = "",
								const std::string & = "") = 0;
				/// authorization debug, with strerror(errno) appended
				virtual void adstr(const std::string &,const std::string & = "",
								const std::string & = "") = 0;
				/// return error message
				virtual const char *err() const=0;
				/// sets client address
				virtual void clip_set(const char *) =0;
};

/**
 * initialize log.
 */
void log_init(clogger **);

#endif
