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

#ifndef __CUIMOD_H
#define __CUIMOD_H

#include <string>
#include <vector>
#include <memory>
#include <cmod.h>
#include "cdbdaemon.h"

/**
 * \ingroup mod
 */
//@{

/**
 * User information.
 */
class cuimod : public cmod {
        protected:
                std::string locale; // localization information (for example pl_PL)
                std::vector<cdbdaemon::edu_info> edu_levs;
                std::vector<cdbdaemon::work_info> works;
                std::vector<cdbdaemon::int_info> ints;
                std::vector<cdbdaemon::area_info> areas;
                std::vector<cdbdaemon::sex_info> sexes;
                cdbdaemon::user_info ui, ui_org;
                cdbdaemon::domain_info dominfo;
                
                std::auto_ptr<cdbdaemon> idb;
                static const char _id[];
                static const char _name[];
                static const char _conf[];

                void act();
                void ints2form();
                void form2ints();
                void post_act();
        public:
                cuimod( cenvironment & );
                const char *id() { return _id; }
                const char *name() { return ""; }
                const char *menu() { return ""; }
                const char *conf() { return _conf; }
                char run();
                virtual ~cuimod() {}
};
//@}
#endif
