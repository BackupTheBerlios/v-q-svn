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

#include "vqwww.h"

#include "cftpmod.h"
#include "cwwwalimod.h"
#include "cusersmod.h"
#include "cuseraddmod.h"
#include "cwwwstatmod.h"

using namespace std;

static void hosting_mods_add( deque<cmod *> & mods, cenvironment & env ) {
    mods.push_back( new cftpmod(env) );
    mods.push_back( new cwwwalimod(env) );
    mods.push_back( new cwwwstatmod(env) );
    mods.push_back( new cusersmod(env) );
    mods.push_back( new cuseraddmod(env) );
}

mod_info hosting_mi = { &hosting_mods_add };
