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

#include "coutmod.h"
#include "vqwww.h"

using namespace std;
using namespace cgicc;

const char coutmod::_id[] = "out";

/**************************************************************************
 *  
 *  ***********************************************************************/
char coutmod::run() {
    const_form_iterator fi = env.cgi_get().getElement("id");
    if( fi != env.cgi_end_get() && fi->getValue() == _id ) {
            return sessclose;
    }
    return notme;
}
