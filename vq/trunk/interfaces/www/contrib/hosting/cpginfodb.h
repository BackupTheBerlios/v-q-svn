/*
Copyright (c) 2003 Pawel Niewiadomski
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

#ifndef __CPGINFODB_H
#define __CPGINFODB_H

#include <stdexcept>
#define HAVE_NAMESPACE_STD 1
#include <pqxx/all.h>
#include <memory>

#include "cinfodb.h"

/**
 * PostgreSQL database support.
 */
class cpginfodb : public cinfodb {
protected:
    static unsigned inst; // number of instances
    static auto_ptr<pqxx::Connection> pg; // of connection for all

    uint8_t lastret;
    void init();
    
    bool wwwali_exe( pqxx::transaction_base &,
			const std::string &, const std::string &, char = 'm');
    bool wwwali_exe( pqxx::transaction_base &,
			const std::string &, const wwwali_info &, char = 'm');

    uint8_t wwwali_dom_get( pqxx::transaction_base &,
			const std::string &, std::string &);
	
    uint8_t wwwali_rdom_get( pqxx::transaction_base &,
			const std::string &, std::string &);
	
    uint8_t wwwali_get( pqxx::transaction_base &,
			const std::string &, wwwali_info &);
	
    std::string dir_clear(const std::string &);

public:
    cpginfodb();
    virtual ~cpginfodb();
    
    uint8_t ftp_add(const std::string &, const ftp_info &);
    uint8_t ftp_rm(const std::string &, const std::string &);
    uint8_t ftp_ls(const std::string &, std::vector<ftp_info> & );
    uint8_t ftp_rep(const std::string &, const ftp_info &);
    uint8_t ftp_get(const std::string &, ftp_info &);
    
    uint8_t wwwali_ls(const std::string &, std::vector<wwwali_dom_info> &);
    uint8_t wwwali_dom_ls(const std::string &, std::vector<wwwali_info> & );
    uint8_t wwwali_dom_ls_dir(const std::string &, const std::string &, std::vector<wwwali_info> & );
    uint8_t wwwali_rep(const std::string &, const wwwali_info & );
    uint8_t wwwali_rm(const std::string &, const std::string &);
    uint8_t wwwali_add(const std::string &, wwwali_info &);
    uint8_t wwwali_get(const std::string &, wwwali_info &);
    uint8_t wwwali_dom_get(const std::string &, std::string &);
    uint8_t wwwali_dom_id_get(const std::string &, std::string &);
    uint8_t wwwali_rdom_get(const std::string &, std::string &);

    uint8_t user_ls(const std::string &, std::vector<user_info> & );

    uint8_t ui_id_get(const std::string &, const std::string &, std::string &);
};
#endif
