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
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <new>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cstdio>
#include <cerrno>

#include "sys.h"
#include "cqautoresp.h"
#include "vq_conf.h"
#include "pfstream.h"

using namespace std;
using namespace posix;
using namespace vq;

cautoresp * cautoresp::alloc() {
	return new cqautoresp();
}

/**
 * 
 */
cqautoresp::cqautoresp() {
	if( !sig_pipe_ign() ) 
			throw runtime_error("can't ignore SIGPIPE");
	
	if( regcomp(&re_x_remark, ".*automatic.*response.*", REG_ICASE | REG_NOSUB 
		| REG_EXTENDED ) )
			throw regex_comp("re_x_remark");
}
/**
 * 
 */
cqautoresp::~cqautoresp() {
	regfree(&re_x_remark);
}

/**
 *
 */
void cqautoresp::msg_set( const string & m ) {
	msg = m;
}

/**
 * 
 */
void cqautoresp::hdrs_add_set( const map_hdr_val & m ) {
	hdrs_add = m;
}

/**
 *
 */
void cqautoresp::msgWrite( ostream & out ) {
	map_hdr_val::const_iterator iter, end;
	map_hdr_val::iterator rp;
	if( (rp=hdrs_add.find("return-path")) != hdrs_add.end() )
			hdrs_add.erase(rp);

	out<<"Return-Path: "<<hdrs_add["from"]<<endl;
	for( iter=hdrs_add.begin(), end=hdrs_add.end(); iter!=end; ++iter ) {
			out<<iter->first<<": "<<iter->second<<endl;
	}
	out<<endl;
	out<<msg;
	out<<endl;
	out.flush();
}

/**
 * send message using qmail-inject
 */
void cqautoresp::msgSend() {
	int qip[2], pid;
	char *args[] = { "qmail-inject", NULL };
    
	if(pipe(qip)) throw runtime_error("pipe failed!");
	switch((pid=fork())) {
	case -1: throw runtime_error("fork failed!");
	case 0:
			close(qip[1]);
			if( dup2(qip[0], 0) != -1 ) {
					execvp(*args, args);
			}
			_exit(111);
	}

	close(qip[0]);
	pfstream out(qip[1]);
	msgWrite(out);
	out.close();
	if(out.bad()) 
			throw runtime_error("qmail-inject write problem");
	while(wait(&pid)==-1 && errno==EINTR);
	if( ! WIFEXITED(pid) || WEXITSTATUS(pid) ) 
			throw runtime_error("qmail-inject crashed or returned!=0");
}

/**
 *
 */
void cqautoresp::reply( const map_hdr_val & const_hdrs ) {
	map_hdr_val hdrs(const_hdrs);
	const char *tmp;
	const char md[] = "mailer-daemon";
	string host, rcpt, local, sender, new_rcpt;

	if(!(tmp=getenv("HOST"))) throw env_miss("HOST");
	host = tmp;
	if(!(tmp=getenv("RECIPIENT"))) throw env_miss("RECIPIENT");
	rcpt = tmp;
	if(!(tmp=getenv("LOCAL"))) throw env_miss("LOCAL");
	local = tmp;
	if(!(tmp=getenv("SENDER"))) throw env_miss("SENDER");
	sender = lower(tmp);

	// don't reply on bounce messages
	if( sender.empty() || sender == "#@[]" 
		|| sender == "<>" || sender == "<#@[]>"
		|| sender.substr(0,sizeof(md)-1) == md ) return;

	// don't reply to messages from mailing lists
	if( hdrs.count("list-id") || hdrs.count("mailing-list")
		|| hdrs.count("x-mailing-list") || hdrs.count("x-ml-name")
		|| hdrs.count("list-help") || hdrs.count("list-unsubscribe")
		|| hdrs.count("list-subscribe") || hdrs.count("list-post")
		|| hdrs.count("list-owner") || hdrs.count("list-archive") ) return;
	
	if( hdrs.count("precedence") ) {
			hdrs["precedence"] = lower(hdrs["precedence"]);
			if( hdrs["precedence"] == "junk" 
				|| hdrs["precedence"] == "bulk"
				|| hdrs["precedence"] == "list" ) return;
	}
	
	// try to get reply address, if it's this message recipient's address 
	// don't reply
	if( (new_rcpt=hdrs["reply-to"]).empty() ) {
			if( (new_rcpt=hdrs["from"]).empty() ) {
					if( (new_rcpt=hdrs["sender"]).empty() ) {
							new_rcpt = sender;
					}
			}
	} 
	if(new_rcpt == rcpt) return;

	// autoresponder found on freshmeat checks this field so try to be
	// compatible
	if( ! regexec(&re_x_remark, hdrs["x-remark"].c_str(), 0, NULL, 0) ) return;
	hdrs_add["x-remark"] = "Automatic response generated by vq's autoresponder";
	
	// subdir tells us where's user's directory
	string dir(subdir(local,host));

	if( histHas(dir+"/.autoresp.senders", sender) ) return;
	if( hdrs.count("references") || hdrs.count("in-reply-to") ) {
			if( histIDMatches(dir+"/.autoresp.senders", hdrs["in-reply-to"], 
					hdrs["references"]) ) return;
	}
	
	// create references field
	hdrs_add["references"] = hdrs.count("references") ? 
			hdrs["message-id"]+", "+hdrs["references"] : hdrs["message-id"];

	// set replied message id
	hdrs_add["in-reply-to"] = hdrs["message-id"];

	// title
	if(!hdrs_add.count("subject"))
			hdrs_add["subject"] = "Automatic response | Odpowiedz automatyczna";

	// standard
	if(!hdrs_add.count("from"))
			hdrs_add["from"] = rcpt.find(host)==0 
					&& rcpt.length() > host.length()+7 
					? rcpt.substr(host.length()+1) : rcpt; 

	hdrs_add["to"] = new_rcpt;
	hdrs_add["message-id"] = (string)"<"+uniq()+".autoresp@"+host+">";
	hdrs_add["mime-version"] = "1.0";
	hdrs_add["content-type"] = codepage.empty() ? "text/plain" 
		: "text/plain; charset="+codepage;
			
	msgSend();

	try {
			if( ! sender.empty() )
					histAdd(dir+"/.autoresp.senders", sender);
			if( hdrs_add.count("message-id") )
					histAdd(dir+"/.autoresp.msg.ids", hdrs_add["message-id"]);
	} catch(...) {
	}
}

/**
 * Local should be in form: sth.pl-user-ext-ext.
 * Host should be in form: sth.pl.
 * Function returns user. If host is not found in local it returns
 * string after first - to second;
 * \return name of a directory containing user's mailbox
 */
string cqautoresp::subdir(const string &local, const string &host) {
    string ret;
    string::value_type hyp;
    string::size_type hostl = host.length(), hyp2pos=string::npos;

    if( ! hostl || local.length() < hostl+2
        || local.find(host) != 0 ) {
            hostl = local.find('-');
            if(hostl != string::npos) {
                    ++hostl;
                    hyp2pos = local.find('-', hostl+1);
            }
    } else {
            hyp = local[hostl++];
            hyp2pos = local.find(hyp, hostl+1);
    }
    if(hostl==string::npos) ret= local;
    else if(hyp2pos == string::npos) ret = local.substr(hostl);
    else ret = local.substr(hostl, hyp2pos-hostl);
    return ret;
}

/**
 * return some value that is uniq on this host
 */
string cqautoresp::uniq() {
    ostringstream u;
    struct timeval tv;
    gettimeofday(&tv,NULL);
	u<<tv.tv_sec<<'.'<<tv.tv_usec<<'.'<<getpid();
	return u.str();
}

/**
 * add information to history file. Don't care if we loose something, but
 * it's important not to destroy history file.
 * \return false there's no valid entry (created in given period of time)
 * \param fn file name
 * \param _add string to add
 * \throw file_write if it can't write to a file
 * \throw file_read if it can't read from a file
 */
bool cqautoresp::histAdd(const std::string &fn, const std::string &_add) {
	bool matches = false;
	string add(_add);
	string fntmp = fn+uniq(), ln;
	istringstream time;
	string::size_type sep;
	opfstream tmp(fntmp.c_str());
	pfstream in(fn.c_str(), ios::in | ios::out);
	struct timeval now, then, sub;
	
	if(!tmp) throw file_write(fntmp);
	if(!in) throw file_write(fn);

	if(gettimeofday(&now, NULL))
			throw runtime_error("gettimeofday failed!");

	replace(add.begin(), add.end(), ' ', '_');
	replace(add.begin(), add.end(), '\n', '_');

	// line looks like this: item time
	while(getline(in, ln)) {
			sep = ln.find(' ');
			if(sep == string::npos) continue; // ignore
			time.str( ln.substr(sep+1) );
			then.tv_usec = 0;
			time>>then.tv_sec;
			if( ! time ) continue;
			timersub(&now, &then, &sub);
			if( sub.tv_sec > ac_autoresp_ttl.val_int() ) continue;
			tmp<<ln<<endl;
			ln=ln.substr(0, sep);
			if(ln == add) matches= true;
	}
	if( ! matches ) {
		tmp<<add<<' '<<now.tv_sec<<endl;
	}
	in.close();
	tmp.close();

	if( in.bad() ) throw file_read(fn);
	if( tmp.bad() ) throw file_write(fntmp);

	if( rename(fntmp.c_str(), fn.c_str()) ) {
			unlink(fntmp.c_str());
			throw file_write(fn);
	} 
	return matches;
}

/**
 * check information in a history file
 * \return false there's no valid entry (created in given period of time)
 * \param fn file name
 * \param _chk string to chk
 * \throw runtime_error thrown if there's error reading file
 */
bool cqautoresp::histHas(const std::string &fn, const std::string &_chk) {
	bool matches = false;
	string chk(_chk);
	string ln;
	istringstream time;
	string::size_type sep;
	ipfstream in(fn.c_str());
	struct timeval now, then, sub;
	
	if(!in) return false;
	if( gettimeofday(&now, NULL) ) throw runtime_error("gettimeofday failed!");

	replace(chk.begin(), chk.end(), ' ', '_');
	replace(chk.begin(), chk.end(), '\n', '_');

	// line looks like this: item time
	while(getline(in, ln)) {
			sep = ln.find(' ');
			if(sep == string::npos) continue; // ignore
			time.str( ln.substr(sep+1) );
			then.tv_usec = 0;
			time>>then.tv_sec;
			if( ! time ) continue;
			timersub(&now, &then, &sub);
			if( sub.tv_sec > ac_autoresp_ttl.val_int() ) continue;
			ln=ln.substr(0, sep);
			if(ln == chk) {
					matches= true;
					break;
			}
	}
	if( in.bad() ) throw file_read(fn);
	return matches;
}
/**
 * check whether id stored in a history file matches any in given strings
 * \return false there's no valid entry (created in given period of time)
 * \param fn file name
 * \param _chk string to chk
 * \param _chk1 2 string to chk
 * \throw runtime_error thrown if there's error reading file
 */
bool cqautoresp::histIDMatches(const std::string &fn, 
		const std::string &_chk, const std::string &_chk1) {
	bool matches = false;
	string chk(_chk), chk1(_chk1);
	string ln;
	istringstream time;
	string::size_type sep;
	ipfstream in(fn.c_str());
	struct timeval now, then, sub;
	
	if(!in) return false;
	if(gettimeofday(&now, NULL)) throw runtime_error("gettimeofday failed!");

	replace(chk.begin(), chk.end(), ' ', '_');
	replace(chk.begin(), chk.end(), '\n', '_');

	replace(chk1.begin(), chk1.end(), ' ', '_');
	replace(chk1.begin(), chk1.end(), '\n', '_');

	// line looks like this: item time
	while(getline(in, ln)) {
			sep = ln.find(' ');
			if(sep == string::npos) continue; // ignore
			time.str( ln.substr(sep+1) );
			then.tv_usec = 0;
			time>>then.tv_sec;
			if( ! time ) continue;
			timersub(&now, &then, &sub);
			if( sub.tv_sec > ac_autoresp_ttl.val_int() ) continue;
			ln=ln.substr(0, sep);
			if( (! chk.empty() && chk.find(ln) != string::npos)
				|| (!chk1.empty() && chk1.find(ln) != string::npos) ) {
					matches= true;
					break;
			}
	}
	if( in.bad() ) throw file_read(fn);
	return matches;
}

/**
 * 
 */
void cqautoresp::codepage_set( const string & cp ) {
	codepage = cp;
} 
