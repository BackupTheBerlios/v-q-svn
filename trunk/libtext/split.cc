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

#include "split.hpp"

#include <string>

namespace text {
	
	using namespace std;
	
	/**
	 * \param str string to split
	 * \param sep separator of substrings
	 * \param rep separator of substrings in returned string
	 * \param cnt maximal number of splits
	 * \param rev ret is in reverse order
	 * \return prefix of string
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
	 * \param str string to split
	 * \param len length of first parts of splitted string
	 * \param rep separator of substrings in returned string
	 * \param cnt maximal number of splits
	 * \param rev ret is in reverse order
	 * \return prefix of string
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
	
	string split_user( const string & u , string::size_type c )
	{
		return split(u,"","/",c);
	}
	
#ifdef SPLITTEST
#include <iostream>
	int cppmain()
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

} // namespace text
