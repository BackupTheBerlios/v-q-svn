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

#ifndef __CUSERSMOD_H
#define __CUSERSMOD_H

#include <string>
#include <functional>
#include <memory>

#include "cvq.h"
#include "cmod.h"
#include "cinfodb.h"

using namespace std;

/**
 * \ingroup mod
 */
//@{

/**
 * Show users.
 */
class cusersmod : public cmod {
        protected:
                auto_ptr<cinfodb> idb;
                static const char _id[];
                static const char _conf[];

                void act();

				/// Comparator for find
                struct _cmp : public unary_function<const cinfodb::user_info &, bool> {
                        string id;
                        result_type operator ()(argument_type ptr) {
                                return ptr.id == id ? true : false;
                        }
                } cmp;
        public:
                cusersmod( cenvironment & );
                const char *id() { return _id; }
				//@{
				/// not used
                const char *name() { return ""; }
                const char *menu() { return ""; }
				//@}
                const char *conf(); 
                char run();
                virtual ~cusersmod() {}
};
//@}

#endif
