/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new-baz.com)

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

#include <iostream>
#include <memory>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

/**
 * arguments:
 * operation (r -remove, m-modify)
 * link name
 * directory
 */
int main( int ac, char **av) {
  if( ac!= 4 ) {
    cerr<<*av<< ": bad number of args"<<endl;
    return 111;
  }
  if( ! *av[2] || ! *av[3] ) {
    cerr<<*av<<": get lost"<<endl;
    return 111;
  }
  const char *alias = av[2], *dir = av[3];
  string aliastmp(av[2]);
  aliastmp+=".tmp";

  switch(*av[1]) {
  case 'm':
    if(symlink(dir, aliastmp.c_str())) {
      cerr<<*av<<": symlink: " << aliastmp <<"->"<<dir<< ": "<<strerror(errno)<<endl;
      return 111;
    }
    if(rename(aliastmp.c_str(), alias)) {
      unlink(aliastmp.c_str());
      cerr<<*av<<": rename: "<<aliastmp<<"->"<<alias<<": "<<strerror(errno)<<endl;
      return 111;
    }
    break;
  case 'r':
    if(unlink(alias) && errno != ENOENT) {
      cerr<<*av<<": unlink: "<<alias<<": "<<strerror(errno)<<endl;
      return 111;
    }
    break;
  default: 
    cerr<<*av<<": unknown operation"<<endl;
    return 111;
  }
  return 0;
}
