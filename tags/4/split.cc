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

#include <string>

#include "split.h"
#include "vq_conf.h"

using namespace std;
using namespace vq;

/**
@param str string to split
@param sep separator of substrings
@param rep separator of substrings in returned string
@param cnt maximal number of splits
@param rev ret is in reverse order
@return prefix of string
*/
string split(const string &str, const string &sep, 
	const string &rep, string::size_type cnt, bool rev ) {
	string ret;
	string::size_type i, len=str.length(), beg=0, pos=0, slen = sep.length();
	if( ! sep.empty() ) {
			for( i=0; i<cnt && i<len; i++, beg = pos+slen ) {
					pos = str.find(sep, beg);
					if( ! rev ) ret += str.substr(beg, pos-beg) + rep;
					else ret = str.substr(beg, pos-beg)+rep+ret;
					if(pos == str.npos) return ret;
			}
	} else {
			for( i=0; i<cnt && i<len; i++, beg = pos+slen ) {
					if( ! rev ) ret += str.substr(i,1) + rep;
					else ret = str.substr(i,1)+rep+ret;
			}
	}
	return ret;
}
/**
@param str string to split
@param len length of first parts of splitted string
@param rep separator of substrings in returned string
@param cnt maximal number of splits
@param rev ret is in reverse order
@return prefix of string
*/
string split(const string &str, string::size_type len, 
	const string &rep, string::size_type cnt, bool rev ) {
	string ret;
	string::size_type i, slen=str.length(), beg=0;

	for( i=0; i<cnt && beg<slen; i++, beg += len ) {
			if( ! rev ) ret += str.substr(beg,len) + rep;
			else ret = str.substr(beg,len)+rep+ret;
	}
	return ret;
}

string split_dom( const string & dom ) {
	return split_dom(dom, ac_split_dom.val_int());
}

string split_dom( const string & str, string::size_type cnt,
		const string &rep )
{
	string ret;
	string::size_type i, len=str.length(), beg=len, pos=0;
	for( i=0; i<cnt && i<len; i++, beg = pos-1 ) {
			pos = str.rfind(".", beg);
			ret += str.substr(pos+1, beg-pos) + rep;
			if(pos == str.npos) return ret;
	}
	return ret;
}

string split_user( const string & u ) {
	return split_user(u, ac_split_user.val_int());
}

string split_user( const string & u , string::size_type c )
{
	return split(u,"","/",c);
}

#ifdef SPLITTEST
#include <iostream>
int main()
{
	string t="d.xxc.a.fg.";
#define test_(x) cout<< t << ' ' << #x << '=' << x << endl;
	test_( split(t,".","/",2) );
	test_( split(t,".","/",4) );
	test_( split(t,".","/",20) );
	test_( split(t,"xxc"," ",5) );
	test_( split(t,1,"/",2) );
	test_( split(t,1,"/",2,true) );
	test_( split(t,3,"/",5) );
	test_( split(t,3,"/",5,true) );
	t+= "xxc.fdg.xxc.sd";
	test_( split(t,"xxc"," ",2) );
	test_( split(t,"","/",2) );
	test_( split(t,""," ",5) );
	t = "test.pl";
	test_( split(t,".","/",3) );
	t = "hosting.sarp.org.pl";
	test_( split_dom(t) );
	test_( split_dom(t, 2) );
	test_( split_dom(t, 0) );
	test_( split_dom(t, 10) );
	t = "Asdasd";
	test_( split_dom(t) );
	return 0;
}
#endif
