/*
Copyright (c) 2002 Pawel Niewiadomski
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
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

int main( int ac, char ** av )
{
    if( ac >= 3 ) {
            int val, mult=1;
            istringstream sval;
            sval.str(av[1]);
            if(av[1][0] == '0') sval>>oct>>val;
            else sval>>val;
            if(!sval) {
                    cerr<<av[1]<<" isn't a number"<<endl;
                    return 4;
            }
            
            if( ac == 4 ) {
                    sval.clear();
                    sval.str(av[3]);
                    sval>>mult;
                    if(! sval) {
                            cerr<<av[3]<<" isn't a number"<<endl;
                            return 5;
                    }
                    val *= mult;
            }

            cout<<"#include \"conf_"<<av[2]<<".h\""<<endl;
            cout<<"const std::string conf_"<<av[2]<<"_s = \"";
            cout << val << "\";\nconst int conf_"<<av[2]<<"_i = "
                 << val << ";" << endl;
            cout.flush();
    } else {
            cerr << "usage: " << *av << " value variable's_name [multiplier]" << endl;
            return 2;
    }
    return 0;
}

