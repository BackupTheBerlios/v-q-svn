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
#ifndef __CMOD_H
#define __CMOD_H

#include <iostream>
#include <vector>

#include "cenvironment.h"

/**
 * \defgroup mod Vqwww modules
 */
//@{

/**
 * Base class for all modules.
 */
class cmod {
        protected:
				cenvironment &env; //!< each module has own environment
				std::ostream &out;
				std::ostream &err;
				cvq & vq;
				const csess_basic & sessb;
				const cgicc::Cgicc &cgi;
				
				/// can't create cmod directly
                cmod( cenvironment & e ) : env(e), out(e.out_get()), 
						err(e.err_get()), vq(e.vq_get()), 
						sessb(e.sessb_get()), cgi(e.cgi_get()) {} 

        public:
                /// codes returned by run()
                enum {
                        loopa, //!< do module check again, but don't end this turn
                        loopaf, //!< loop again, after finishing this
                        notme, //!< i did nothing
                        done,    //!< this module was selected, action was taken, you can exit
                        sessclose //!< close session (for example user wants to log out)
                };
               
                /// flags used by modules to report their capatibilities
                enum _modinfo {
                        mi_none = 0, //!< i do nothing
                        mi_conf = 1, //!< i'm configurable, cconfmod will use it
                        mi_menu = 2 //! put me in menu
                };
                
                /// information about module's capatibilities
                unsigned modinfo;

                virtual const char *id()=0; //!< return module's id, usually it shoud be class name without prefix 'c', and postfix "mod"
                virtual const char *name()=0; //!< at this time not used
                virtual const char *menu()=0; //!< xml code of menu's entry
                virtual const char *conf()=0; //!< xml code of configuration's entry
                virtual char run()=0; //!< perform module's action
};

//@}

#endif
