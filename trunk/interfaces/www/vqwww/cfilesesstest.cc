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

#include "cfilesesstest.h"

void cfilesesstest::run()
{
        string csid;
        bool t;
        test_( fs.create(csid) );
        test_( fs.setval("test", "asdasd\nasdasd") );
        test_( fs.getval("test", csid) );
        test_( csid == "asdasd\nasdasd" ); 
        test_( fs.setval("test", "xcvcxv\na32545sdasd\nasdasd") );
        test_( fs.getval("test", csid) );
        test_( csid == "xcvcxv\na32545sdasd\nasdasd" );
        test_( fs.rmval("test") );
        test_( ! fs.getval("test", csid) );
        test_( fs.setval("bu", true) );
        test_( fs.getval("bu", t) );
        test_( t );
        test_( fs.rm() );
}
