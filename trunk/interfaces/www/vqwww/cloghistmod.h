/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2005 Pawel Niewiadomski (new@foo-baz.com)

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

#ifndef __CLOGHISTMOD_H
#define __CLOGHISTMOD_H

#include "cmod.h"
#include <clogger.h>

/**
 * \ingroup mod
 */
//@{

/**
 * Show login history.
 */
class cloghistmod : public cmod {
        protected:
                static const char _id[];
                static const char _menu[];

                void act( clogger * );

				/// Gets start and cnt from GET request
				void range_get( clogger::size_type &, clogger::size_type & );

				/// Dumps browsing menu (next, previous, etc.)
				void navi_dump(clogger::size_type, clogger::size_type, 
						clogger::size_type );

        public:
                cloghistmod(cenvironment &e) : cmod(e) { modinfo=mi_menu; }
                const char *id() { return _id; }
                const char *name() { return ""; }
                const char *menu() { return _menu; }
                const char *conf() { return ""; }
                char run();
};
//@}
#endif
