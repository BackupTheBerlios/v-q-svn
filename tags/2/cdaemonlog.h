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

#ifndef __CDAEMONLOG_H
#define __CDAEMONLOG_H

#include <stdexcept>
#include <sys/types.h>
#include <sys/un.h>
#include <string>
#include <inttypes.h>

#include "clog.h"

/**
 * Sending logging information to daemon that stores it in some medium
 * (for example database system)
 */
class cdaemonlog : public clogger {
        protected:
                int cso;
                struct sockaddr_un cun;
                std::string errmsg, clip;
               
                /// write command to daemon
                uint8_t send(uint8_t, const std::string &);
                uint8_t send(uint8_t, const std::string &, const std::string &);
                uint8_t asend(uint8_t, const std::string &, const std::string &, const std::string &);
                uint8_t asend(uint8_t, const std::string &, const std::string &,
                                const std::string &, const std::string &);
        public:
                cdaemonlog() throw(std::runtime_error);
                virtual ~cdaemonlog();
                
                /// commands
                enum cmd {
                        /// error message
                        cmd_e=0,
                        /// error message 2 std::strings
                        cmd_estr,
                        /// warning
                        cmd_w,
                        /// warning, 2 std::strings
                        cmd_wstr,
                        /// notice
                        cmd_n,
                        /// notice, 2 std::strings
                        cmd_nstr,
                        /// debug
                        cmd_d,
                        /// debug, 2 std::strings
                        cmd_dstr,
                        /// auth. error message
                        cmd_ae,
                        /// auth. error message 2 std::strings
                        cmd_aestr,
                        /// auth. warning
                        cmd_aw,
                        /// auth. warning, 2 std::strings
                        cmd_awstr,
                        /// auth. notice
                        cmd_an,
                        /// auth. notice, 2 std::strings
                        cmd_anstr,
                        /// auth. debug
                        cmd_ad,
                        /// auth. debug, 2 std::strings
                        cmd_adstr
                };
                
                /// error message
                void e(const std::string &) throw();
                /// error message, function ads strerror(errno) message at end of std::string
                void estr(const std::string &) throw();
                /// warning
                void w(const std::string &) throw();
                /// warning, with strerror(errno) appended
                void wstr(const std::string &) throw();
                /// notice, info
                void n(const std::string &) throw();
                /// 
                void nstr(const std::string &) throw();
                /// debug
                void d(const std::string &) throw();
                ///
                void dstr(const std::string &) throw();
                
                /// error message
                void ae(const std::string &,const std::string & = "",
                                const std::string & = "") throw();
                /// error message, function ads strerror(errno) message at end of std::string
                void aestr(const std::string &,const std::string & = "",
                                const std::string & = "") throw();
                /// warning
                void aw(const std::string &,const std::string & = "",
                                const std::string & = "") throw();
                /// warning, with strerror(errno) appended
                void awstr(const std::string &,const std::string & = "",
                                const std::string & = "") throw();
                /// notice, info
                void an(const std::string &,const std::string & = "",
                                const std::string & = "") throw();
                /// 
                void anstr(const std::string &,const std::string & = "",
                                const std::string & = "") throw();
                /// debug
                void ad(const std::string &,const std::string & = "",
                                const std::string & = "") throw();
                ///
                void adstr(const std::string &,const std::string & = "",
                                const std::string & = "") throw();
                ///
                const char *err() const { return errmsg.c_str(); }
                /// sets client ip
                void clip_set(const char * c) { if(c) clip = c; }
};

#endif
