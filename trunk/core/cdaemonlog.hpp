/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new-baz.com)

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

};

#endif
