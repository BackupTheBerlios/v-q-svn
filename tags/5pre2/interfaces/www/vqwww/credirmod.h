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


#ifndef __CREDIRMOD_H
#define __CREDIRMOD_H

#include <string>
#include <functional>

#include "cvq.h"
#include "cmod.h"

/**
 * \ingroup mod
 */
//@{

/**
 * Setup e-mail redirections.
 */
class credirmod : public cmod {
        protected:
                static const char _id[];
                static const char _conf[];

                void act();
				void list( std::vector<cvq::udot_info> & );
				void post_read( std::vector<cvq::udot_info>::size_type & cnt,
						std::vector<cvq::udot_info> &, 
						std::vector<cvq::udot_info> &, 
						std::vector<cvq::udot_info> & );

				void post( std::vector<cvq::udot_info> &,
						std::vector<cvq::udot_info>::size_type & );
				
				/// Comparator for find
                struct _cmp : public std::unary_function<const cvq::udot_info &, bool> {
                        std::string id;
                        result_type operator ()(argument_type ptr) {
                                return ptr.id == id ? true : false;
                        }
                } cmp;

                void rmspace(std::string &);
        public:
                credirmod( cenvironment & e ) : cmod(e) { modinfo=mi_conf; }
                const char *id() { return _id; }
                const char *name() { return ""; }
                const char *menu() { return ""; }
                const char *conf() { return _conf; }
                char run();
                virtual ~credirmod() {}
};
//@}
#endif
