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
#include <new>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#include "cpginfodb.h"
#include "util.h"
#include "lower.h"
#include "hosting_conf.h"

using namespace std;
using namespace hosting;

cinfodb * cinfodb::alloc() {
	return new cpginfodb();
}

#define lreturn(x) do { uint8_t ret = x; \
        if(ret!=err_no) lastret = ret; return ret; } while(0)

unsigned cpginfodb::inst = 0; // number of instances
auto_ptr<pqxx::Connection> cpginfodb::pg;

/**
 *
 */
void cpginfodb::init() {
  lastret = err_no;
}
/**
 *
 */
cpginfodb::cpginfodb() {
  init();
  if( ! inst )
        pg.reset (new pqxx::Connection(ac_pgsql.val_str().c_str()));
  ++inst;
}
/**
 *
 */
cpginfodb::~cpginfodb() {
    if(!inst) throw runtime_error("cpginfodb::~cpginfodb");
    --inst;
    if( ! inst ) pg.reset(NULL);
}

/**
 *
 */
uint8_t cpginfodb::ftp_ls(const string &d, vector<ftp_info> &users) {
	string dom(d);
	str2tb(dom);
	users.clear();

	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT id,login,ftpsubdir FROM \""+dom+"\" WHERE login!=''"
		" AND login IS NOT NULL AND flags & 20 = 0 ORDER BY login") );
	
	int tup = res.size();
	if(!tup) lreturn(err_no);
	users.reserve(tup);
	ftp_info fi;
	
	const char *ptr;
	for( int i=0; i < tup; ++i ) {
			ptr = res[i][0].c_str();
			if(!ptr) continue;
			fi.id = ptr;
			ptr = res[i][1].c_str();
			if(!ptr) continue;
			fi.user = ptr; 
			fi.dir = (ptr=res[i][2].c_str()) ? ptr : "";
			users.push_back(fi);
	}
	lreturn(err_no);
}

/**
 * \param info info.user should contain user name
 */
uint8_t cpginfodb::ftp_get(const string &d, ftp_info &info) {
	string dom(d);
	str2tb(dom);

	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT id,ftpsubdir FROM \""+dom+"\" WHERE login="
		+pqxx::Quote(info.user, false)) );
	
	const char *ptr;
	int tup = res.size();

	if(!tup) lreturn(err_no);

	ptr = res[0][0].c_str();
	if(!ptr || !*ptr) lreturn(err_noent);
	info.id = ptr;
	info.dir = (ptr=res[0][1].c_str()) ? ptr : "";
	lreturn(err_no);
}



/**
 *
 */
uint8_t cpginfodb::ftp_add(const string &d, const ftp_info &fi) {
	string dom(d), domlow(lower(d));
	str2tb(dom);

	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"UPDATE \""+dom+"\" SET ftpdir="
		+pqxx::Quote(ac_ftp_base.val_str()+'/'+domlow+"/www/"+domlow+'/'
				+escape(fi.dir), false)
		+",ftpsubdir="+pqxx::Quote(dir_clear(fi.dir), false)
		+" WHERE id="+pqxx::Quote(fi.id, false)) );
	
	if( ! res.AffectedRows() ) lreturn(err_noent);
	lreturn(err_no);
}

/**
 *
 */
uint8_t cpginfodb::ftp_rm(const string &d, const string &id) {
	string dom(d);
	str2tb(dom);

	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"UPDATE \""+dom+"\" SET ftpdir=NULL,ftpsubdir=NULL"
		" WHERE id="+pqxx::Quote(id, false)) );
	
	if( ! res.AffectedRows() ) lreturn(err_noent);
	lreturn(err_no);
}

/**
 *
 */
uint8_t cpginfodb::ftp_rep(const string &d, const ftp_info &fi) {
	return ftp_add(d, fi);
}

/**
 *
 */
uint8_t cpginfodb::wwwali_ls(const string &dom, vector<wwwali_dom_info> &doms) {
	doms.clear();
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT dw.id,dw.name FROM domains as d,domains_all as dw WHERE "
		"d.id=dw.domain_id AND d.domain="+pqxx::Quote(lower(dom), false)
		+" ORDER BY dw.name") );

	vector<wwwali_dom_info>::size_type cnt, i;
	wwwali_dom_info wadi;
	
	const char *ptr;
	if( ! (cnt=res.size()) ) return err_no;
	
	doms.reserve(cnt);
	for( i=0; i<cnt; ++i ) {
			ptr = res[i][0].c_str();
			if(!ptr) continue;
			wadi.id = ptr;
			
			ptr = res[i][1].c_str();
			if(!ptr || !*ptr) continue;
			wadi.domain = ptr;
			
			doms.push_back(wadi);
	}
	lreturn(err_no);
}

/**
 * gets prefixes for specified domain
 * \return err_read - on sql error, err_no - success
 */
uint8_t cpginfodb::wwwali_dom_ls(const string &id, vector<wwwali_info> & wais ) {
	wais.clear();
	
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT id,prefix,dir FROM domains_prefixes WHERE domain_id="
		+pqxx::Quote(id, false)+" ORDER BY prefix") );
	
	vector<wwwali_info>::size_type i, cnt;
	wwwali_info wai;
	const char *ptr;
	
	if( ! (cnt=res.size()) ) lreturn(err_no);
	
	for( i=0; i<cnt; i++ ) {
			ptr = res[i][0].c_str();
			if(!ptr) continue;
			wai.id = ptr;

			ptr = res[i][1].c_str();
			if(!ptr || ! *ptr) continue;
			wai.prefix = ptr;

			ptr = res[i][2].c_str();
			if(!ptr || ! *ptr) continue;
			wai.dir = ptr;

			wais.push_back(wai);
	}
	lreturn(err_no);
}

/**
 *
 */
uint8_t cpginfodb::wwwali_rep(const string &domid, const wwwali_info & w) {
	wwwali_info waiold;
	waiold.id = w.id;
	if(wwwali_get(domid, waiold)) lreturn(err_read);

	pqxx::Transaction tran(*pg);

	wwwali_info wai(w);
	wai.prefix = lower(wai.prefix);
	wai.dir = dir_clear(wai.dir);

	pqxx::Result res(tran.Exec(
		"UPDATE domains_prefixes SET prefix="+pqxx::Quote(wai.prefix, false)
		+",dir="+pqxx::Quote(wai.dir, false)+" WHERE id="
		+pqxx::Quote(wai.id, false)+" AND domain_id="
		+pqxx::Quote(domid, false)) );

	if( ! res.AffectedRows() ) {
			lreturn(err_noent); // transaction will be aborted
	}

    // calls function writing info about domains to file which is processed
    // to create dns configuration
	tran.Exec("SELECT domains_prefixes_dump()");
	
	if(!wwwali_exe(tran, domid, wai)) {
			lreturn(err_write);
	}

	tran.Commit();

	pqxx::NonTransaction nt(*pg);
	if( wai.prefix != waiold.prefix )
			wwwali_exe(nt, domid, waiold, 'r');
	lreturn(err_no);
}

/**
 *
 */
uint8_t cpginfodb::wwwali_rm(const string &domid, const string &id) {
	pqxx::NonTransaction notrans(*pg);
	
	if( ! wwwali_exe(notrans, domid, id, 'r') ) {
			lreturn(err_write);
	}

	pqxx::Result res(notrans.Exec(
		"DELETE FROM domains_prefixes WHERE id="
		+pqxx::Quote(id, false)+" AND domain_id="+pqxx::Quote(domid, false)) );

    if( ! res.AffectedRows() ) lreturn(err_noent);

	// calls function writing info about domains to file which is processed
	// to create dns configuration
	notrans.Exec("SELECT domains_prefixes_dump()");
	lreturn(err_no);
}

/**
 *
 */
uint8_t cpginfodb::wwwali_add(const string &domid, wwwali_info &wai) {
	wai.prefix = lower(wai.prefix);
	pqxx::Transaction trans(*pg);
	pqxx::Result res(trans.Exec(
		"SELECT nextval('domains_prefixes_id_seq')") );
	
	if( res.size() != 1 ) lreturn(err_read);
	const char *ptr = res[0][0].c_str();
	if(!ptr) lreturn(err_read);
	wai.id = ptr;

	res = trans.Exec("INSERT INTO domains_prefixes (id,prefix,dir,domain_id)"
		" VALUES("+pqxx::Quote(wai.id, false)+","
		+pqxx::Quote(wai.prefix, false)
		+","+pqxx::Quote(dir_clear(wai.dir), false)+","
		+pqxx::Quote(domid, false)+")");

	if( res.AffectedRows() != 1 ) lreturn(err_write);
    
	// calls function writing info about domains to file which is processed
	// to create dns configuration
	trans.Exec("SELECT domains_prefixes_dump()");

	if( ! wwwali_exe(trans, domid, wai) ) {
			lreturn(err_write);
	}

	trans.Commit();
	lreturn(err_no);
}

/**
 *
 */
bool cpginfodb::wwwali_exe(pqxx::transaction_base & trans,
		const string &domid, const string &id, char act ) {
	wwwali_info wai;
	wai.id = id;
	if(wwwali_get(trans, domid, wai)) return false;
	return wwwali_exe(trans, domid, wai, act);
}
/**
 *
 */
bool cpginfodb::wwwali_exe(pqxx::transaction_base & trans,
		const string &domid, const wwwali_info &wai, char act ) {
	char *av[5];
	int r, wstat, pid;
	string domain, rdomain;
	if( wwwali_dom_get(trans, domid, domain)
		|| wwwali_rdom_get(trans, domid, rdomain) ) return false;
	
	string alias(ac_ftp_base.val_str()+'/'+rdomain+"/www/"+wai.prefix+'.'+domain);
	string dir(ac_ftp_base.val_str()+'/'+rdomain+"/www/"+rdomain+'/'+wai.dir);

	string wwwali_exe(ac_wwwali_exe.val_str());
	av[0] = const_cast<char*>(wwwali_exe.c_str());
	av[1] = &act;
	av[2] = (char *)alias.c_str();
	av[3] = (char *)dir.c_str();
	av[4] = NULL;
	switch((pid=vfork())) {
	case -1: return false;
	case 0:
			 close(0);
			 execvp(*av, av);  
			 _exit(1);
	}
 
	while( (r=waitpid(pid, &wstat, 0)) == -1 && errno == EINTR );
	if( r != pid ) return false;
	if( WIFEXITED(wstat) && ! WEXITSTATUS(wstat) ) return true;
	return false;
}

/**
 *
 */
uint8_t cpginfodb::wwwali_get( pqxx::transaction_base & trans, 
		const string &domid, wwwali_info & wai ) {
	pqxx::Result res(trans.Exec(
		"SELECT prefix,dir FROM domains_prefixes WHERE domain_id="
		+pqxx::Quote(domid, false)+" AND id="+pqxx::Quote(wai.id, false)) );

	const char *ptr;
	if( ! res.size() ) lreturn(err_noent);

	ptr = res[0][0].c_str();
	if(!ptr || ! *ptr) lreturn(err_noent);
	wai.prefix = ptr;
	
	ptr = res[0][1].c_str();
	if(!ptr || ! *ptr) lreturn(err_noent);
	wai.dir = ptr;
	
	lreturn(err_no);
}

/**
 *
 */
uint8_t cpginfodb::wwwali_get(const string &domid, wwwali_info & wai ) {
	pqxx::NonTransaction nt(*pg);
	return wwwali_get( nt, domid, wai );
}

/**
 * gets prefixes for specified subdir
 * \return err_read - on sql error, err_no - success
 */
uint8_t cpginfodb::wwwali_dom_ls_dir(const string &id, const string &dir, 
		vector<wwwali_info> & wais ) {
	string edir = dir;
	string::size_type slash, edirl=edir.length();

	for(slash=0; slash<edirl; ++slash)
			if( edir[slash] != '/' ) break;
	if( slash ) edir = edir.substr(slash);
	edirl = edir.length();
	if( edirl ) {
			while( edirl >= 1 && edir[edirl-1] == '/' ) edirl--; 
			edir = edir.substr(0, edirl);
	}
	edir = pqxx::Quote(dir_clear(edir), false);

	wais.clear();
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT id,prefix,dir FROM domains_prefixes WHERE domain_id="
		+pqxx::Quote(id, false)+" AND (dir="+edir+" OR dir='/' || " + edir 
		+" OR dir='/' || "+edir+" || '/' OR dir="+edir+" || '/') "
		"ORDER BY prefix") );

	vector<wwwali_info>::size_type i, cnt;
	wwwali_info wai;
	const char *ptr;

	if( ! (cnt=res.size()) ) lreturn(err_no);

	for( i=0; i<cnt; i++ ) {
			ptr = res[i][0].c_str();
			if(!ptr) continue;
			wai.id = ptr;

			ptr = res[i][1].c_str();
			if(!ptr || ! *ptr) continue;
			wai.prefix = ptr;

			ptr = res[i][2].c_str();
			if(!ptr || ! *ptr) continue;
			wai.dir = ptr;
			
			wais.push_back(wai);
	}

	lreturn(err_no);
}

/**
 *
 */
uint8_t cpginfodb::wwwali_dom_get( pqxx::transaction_base & trans,
		const string &domid, string &domain ) {
	pqxx::Result res(trans.Exec(
		"SELECT name FROM domains_all WHERE id="+pqxx::Quote(domid, false)) );
    
	if( ! res.size() ) lreturn(err_noent);

	const char *ptr = res[0][0].c_str();
	if(! ptr || ! *ptr) lreturn(err_noent);
	domain = ptr;
	lreturn(err_no);
}

/**
 *
 */
uint8_t cpginfodb::wwwali_dom_get(const string &domid, string &domain ) {
	pqxx::NonTransaction nt(*pg);
	return wwwali_dom_get( nt, domid, domain );    
}

/**
 *
 */
uint8_t cpginfodb::wwwali_dom_id_get(const string &domain, string &id ) {
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT id FROM domains_all WHERE name="
		+pqxx::Quote(lower(domain), false)) );

	if( ! res.size() ) lreturn(err_noent);

	const char *ptr = res[0][0].c_str();
	if(! ptr || ! *ptr) lreturn(err_noent);
	id = ptr;
	lreturn(err_no);
}

/**
 * gets main domain name which has alias with given id (from domains_all)
 * \param domid id. from domains_all
 * \param domain main domain name from domains
 */
uint8_t cpginfodb::wwwali_rdom_get( pqxx::transaction_base &trans,
		const string &domid, string &domain ) {

	pqxx::Result res(trans.Exec(
		"SELECT d.domain FROM domains as d,domains_all"
		" as dw where dw.id="+pqxx::Quote(domid, false)
		+" and dw.domain_id=d.id") );
	
	if( ! res.size() ) lreturn(err_noent);
	
	const char *ptr = res[0][0].c_str();
	if(! ptr || ! *ptr) lreturn(err_noent);
	domain = ptr;
	lreturn(err_no);
}

/**
 * gets main domain name which has alias with given id (from domains_all)
 * \param domid id. from domains_all
 * \param domain main domain name from domains
 */
uint8_t cpginfodb::wwwali_rdom_get( const string &domid, string &domain ) {
	pqxx::NonTransaction nt(*pg);
	return wwwali_rdom_get( nt, domid, domain );
}

/**
 *
 */
uint8_t cpginfodb::user_ls(const string &d, vector<user_info> &users) {
	string dom(d);
	str2tb(dom);
	users.clear();

	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT id,login,date_crt,date_exp FROM \""
		+dom+"\" WHERE login!='' AND login IS NOT NULL"
		" AND flags & 20 = 0 ORDER BY login") );

	int tup = res.size();
	if(!tup) lreturn(err_no);
	
	users.reserve(tup);
	user_info fi;
	const char *ptr;
	
	for( int i=0; i < tup; ++i ) {
			ptr = res[i][0].c_str();
			if(!ptr) continue;
			fi.id = ptr;

			ptr = res[i][1].c_str();
			if(!ptr) continue;
			fi.user = ptr; 

			fi.date_crt = (ptr=res[i][2].c_str()) ? ptr : "";
			fi.date_exp = (ptr=res[i][3].c_str()) ? ptr : "";

			users.push_back(fi);
	}
	lreturn(err_no);
}
/**
 * gets user's id from special table including some extra info (client
 * information - address, phone, etc.)
 */
uint8_t cpginfodb::ui_id_get(const string &dom, const string &user, string &id) {
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT id FROM users_info WHERE login="
		+pqxx::Quote(lower(user), false)+" AND domain_id="
        "(SELECT id FROM domains WHERE domain="
		+pqxx::Quote(lower(dom), false)+" LIMIT 1)") );
            
	if( ! res.size() ) lreturn(err_noent);

	const char *tmp = res[0][0].c_str();
	id = ! tmp ? "" : tmp;
	lreturn(err_no);
}

/**
 * remove all multiplied / entries 
 * \param dir string to be cleared
 * \return cleared version
 */
string cpginfodb::dir_clear( const string & dir ) {
    string dirclean;
    string::const_iterator dircur, dirend;
    char state=0;

    dirclean.reserve(dir.length());
    for( dircur=dir.begin(), dirend=dir.end(); dircur!=dirend; ++dircur ) {
            switch(state) {
            case 1:
                if(*dircur == '/') break;
                state = 0;
                dirclean += *dircur;
                break;
            case 0:
                if(*dircur == '/') state = 1;
                dirclean += *dircur;
                break;
            }
    }
    return dirclean; 
}
