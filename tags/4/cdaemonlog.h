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

#include "clogger.h"

/**
 * Sending logging information to daemon that stores it in some medium
 * (for example database system)
 */
class cdaemonlog : public clogger {
		protected:
				int cso; //!< Client's socket
				char resp; //!< Common variable
				char cmd; //!< Common variable
				struct sockaddr_un cun; //!< Client's address
			   
				/// Common part between dom_log_read_log, log_read_dom_log
				uint8_t read_common( std::vector<entry_type> &, 
						size_type, size_type );

				/// Common part between dom_log_cnt_log, log_cnt_dom_log
				uint8_t cnt_common( size_type & );
			
		public:
				cdaemonlog();
				virtual ~cdaemonlog();
				
				/// commands
				enum cmd {
						cmd_write=0, //!< store information
						cmd_dom_log_read_log, //!< read history for domain
						cmd_dom_log_cnt_log, //!< count entries in history for domain
						cmd_log_read_dom_log, //!< read table log
						cmd_log_cnt_dom_log //!< count entries in table log for given domain and login
				};

				uint8_t write( result , const std::string & );
				uint8_t dom_log_read_log( std::vector<entry_type> &, size_type, size_type );
				uint8_t dom_log_cnt_log( size_type & );
				uint8_t log_read_dom_log( std::vector<entry_type> &, size_type, size_type );
				uint8_t log_cnt_dom_log( size_type & );
};

#endif
