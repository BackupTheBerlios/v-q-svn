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
