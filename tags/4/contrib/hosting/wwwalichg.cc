/*
Copyright (c) 2003 Pawel Niewiadomski
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
