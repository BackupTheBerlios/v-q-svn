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
