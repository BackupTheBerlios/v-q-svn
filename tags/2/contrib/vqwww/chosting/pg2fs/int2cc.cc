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
    if( ac >= 2 ) {
            string ln;
            if( getline(cin,ln) ) {
                    if( ac == 3 ) {
                            istringstream in;
                            int mult, num;
                            in.str(av[2]);
                            in>>mult;
                            if(!in) return 4;
                            //in.clear();
                            in.str(ln);
                            in>>num;
                            if(!in) return 5;
                            ostringstream out;
                            out<<num*mult;
                            if(!out) return 6;
                            ln = out.str();
                    }
                    cout<<"#include \"conf_"<<av[1]<<".h\""<<endl;
                    cout<<"const string conf_"<<av[1]<<"_s = \"";
                    cout << ln << "\";\nconst int conf_"<<av[1]<<"_i = "
                         << ln << ";" << endl;
            } else return 3;
            cout.flush();
            if( ! cout ) {
                    return 1;
            }
    } else {
            cerr << "usage: " << *av << " variable's_name [multiplier]" << endl;
            return 2;
    }
    return 0;
}

