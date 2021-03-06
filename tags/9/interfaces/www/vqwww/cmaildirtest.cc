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

#include <stdexcept>

#include "cmaildirtest.h"

void cmaildirtest::run()
{
        test_( md.create("/tmp/testmd") );
        test_( md.open("/tmp/testmd") );
        test_( md.rm() );
/*        
        test_( md.create("/tmp/testmd", true ) );
        test_( md.close() );
        test_( md.open("/tmp/testmd") );
        test_( md.rm() );
        
        test_( ! md.open("/nosuchfileordirectory/testmd") );
        test_( md.errmsg() != "" );
        try {
                md.rm();
                fail_("md.rm with no opened maildir");
        } catch( const logic_error & e ) {
                succeed_();
        }*/
}
