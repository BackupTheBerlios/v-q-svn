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

#ifndef __CWWWSTATMOD_H
#define __CWWWSTATMOD_H

#include <string>
#include <functional>
#include <memory>

#include "cvq.h"
#include "cmod.h"
#include "cinfodb.h"

using namespace std;

/**
 * WWW stats redirector.
 */
class cwwwstatmod : public cmod {
        protected:
                auto_ptr<cinfodb> idb;
                static const char _id[];
                static const char _conf[];
                static const char _name[];
                static const char _menu[];
                
                std::string domains, domain;
                bool empty; //!< there's no stats for this user?

                void act();
                void act_dom(const std::string &);
                void act_dom_dir(const std::string &, const std::string &);
                void act_user();

                void item_dump( const std::string & );
        public:
                cwwwstatmod( cenvironment & env );
                const char *id() { return _id; }
                const char *name() { return _name; }
                const char *menu() { return _menu; }
                const char *conf() { return _conf; }
                char run();
                virtual ~cwwwstatmod() {}
};

#endif
