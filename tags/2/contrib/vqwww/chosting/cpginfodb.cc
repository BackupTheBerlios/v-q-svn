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
#include "conf_pgsql.h"
#include "conf_ftp_base.h"
#include "util.h"
#include "lower.h"
#include "conf_wwwali_exe.h"

using namespace std;

#define lreturn(x) do { uint8_t ret = x; \
        if(ret!=err_no) lastret = ret; return ret; } while(0)

unsigned cpginfodb::inst = 0; // number of instances
auto_ptr<PgDatabase> cpginfodb::pg = auto_ptr<PgDatabase> (NULL);

/**
 *
 */
void cpginfodb::init() {
  lastret = err_no;
}
/**
 *
 */
cpginfodb::cpginfodb() throw(runtime_error) {
  init();
  if( ! inst )
        pg.reset (new PgDatabase(conf_pgsql.c_str()));
  if(pg->ConnectionBad()) {
        pg.reset( new PgDatabase(conf_pgsql.c_str()));
        if(pg->ConnectionBad())
                throw runtime_error((string)"cpginfodb: can't connect to PostgreSQL: "+pg->ErrorMessage());
  }
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
uint8_t cpginfodb::ftp_ls(const string &d, vector<ftp_info> &users) throw() {
  string dom(d);
  str2tb(dom);
  users.clear();
  if( pg->Exec(("SELECT id,login,ftpsubdir FROM "
          +dom+" WHERE login!='' AND login IS NOT NULL"
          " AND flags & 20 = 0 ORDER BY login").c_str()) == PGRES_TUPLES_OK ) {
    int tup = pg->Tuples();
    if(!tup) lreturn(err_no);
    users.reserve(tup);
    ftp_info fi;
    const char *ptr;
    for( int i=0; i < tup; ++i ) {
      ptr = pg->GetValue(i,0);
      if(!ptr) continue;
      fi.id = ptr;
      ptr = pg->GetValue(i, 1);
      if(!ptr) continue;
      fi.user = ptr; 
      fi.dir = (ptr=pg->GetValue(i, 2)) ? ptr : "";
      users.push_back(fi);
    }
    lreturn(err_no);
  }
  lreturn(err_read);
}

/**
 * \param info info.user should contain user name
 */
uint8_t cpginfodb::ftp_get(const string &d, ftp_info &info) throw() {
  string dom(d);
  str2tb(dom);
  
  if( pg->Exec(("SELECT id,ftpsubdir FROM "
          +dom+" WHERE login='"+escape(info.user)+"'").c_str()) == PGRES_TUPLES_OK ) {
    const char *ptr;
    int tup = pg->Tuples();
    
    if(!tup) lreturn(err_no);
    ptr = pg->GetValue(0,0);
    if(!ptr || !*ptr) lreturn(err_noent);
    info.id = ptr;
    info.dir = (ptr=pg->GetValue(0, 1)) ? ptr : "";
    lreturn(err_no);
  }
  lreturn(err_read);
}



/**
 *
 */
uint8_t cpginfodb::ftp_add(const string &d, const ftp_info &fi) {
  string dom(d), domle(escape(lower(d)));
  str2tb(dom);
  if( pg->Exec(("UPDATE "+dom+" SET ftpdir='"+escape(conf_ftp_base)+'/'
          +domle+"/www/"+domle+'/'
          +escape(fi.dir)+"',ftpsubdir='"+escape(dir_clear(fi.dir))+"'"
          " WHERE id='"+escape(fi.id)+"'").c_str()) == PGRES_COMMAND_OK ) {
    if( ! pg->CmdTuples() ) lreturn(err_noent);
    lreturn(err_no);
  }
  lreturn(err_write);
}

/**
 *
 */
uint8_t cpginfodb::ftp_rm(const string &d, const string &id) {
  string dom(d);
  str2tb(dom);
  if( pg->Exec(("UPDATE "+dom+" SET ftpdir=NULL,ftpsubdir=NULL"
      " WHERE id='"+escape(id)+"'").c_str()) == PGRES_COMMAND_OK ) {
     if( ! pg->CmdTuples() ) lreturn(err_noent);
     lreturn(err_no);
  }
  lreturn(err_write);
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
  if( pg->Exec(("SELECT dw.id,dw.name FROM domains as d,domains_all as dw WHERE "
      "d.id=dw.domain_id AND d.domain='"+escape(lower(dom))+"'"
      " ORDER BY dw.name").c_str()) == PGRES_TUPLES_OK ) {
    vector<wwwali_dom_info>::size_type cnt, i;
    wwwali_dom_info wadi;
    const char *ptr;
    if( ! (cnt=pg->Tuples()) ) return err_no;
    doms.reserve(cnt);
    for( i=0; i<cnt; ++i ) {
      ptr = pg->GetValue(i,0);
      if(!ptr) continue;
      wadi.id = ptr;
      ptr= pg->GetValue(i,1);
      if(!ptr || !*ptr) continue;
      wadi.domain = ptr;
      doms.push_back(wadi);
    }
    lreturn(err_no);
  }
  lreturn(err_read);
}

/**
 * gets prefixes for specified domain
 * \return err_read - on sql error, err_no - success
 */
uint8_t cpginfodb::wwwali_dom_ls(const string &id, vector<wwwali_info> & wais ) {
  wais.clear();
  if( pg->Exec(("SELECT id,prefix,dir FROM domains_prefixes WHERE domain_id='"
      +escape(id)+"' ORDER BY prefix").c_str()) == PGRES_TUPLES_OK ) {
    vector<wwwali_info>::size_type i, cnt;
    wwwali_info wai;
    const char *ptr;
    if( ! (cnt=pg->Tuples()) ) lreturn(err_no);
    for( i=0; i<cnt; i++ ) {
      ptr = pg->GetValue(i, 0);
      if(!ptr) continue;
      wai.id = ptr;
      ptr = pg->GetValue(i, 1);
      if(!ptr || ! *ptr) continue;
      wai.prefix = ptr;
      ptr = pg->GetValue(i, 2);
      if(!ptr || ! *ptr) continue;
      wai.dir = ptr;
      wais.push_back(wai);
    }
    lreturn(err_no);
  }
  lreturn(err_read);
}

/**
 *
 */
uint8_t cpginfodb::wwwali_rep(const string &domid, const wwwali_info & w) {
  wwwali_info waiold;
  waiold.id = w.id;
  if(wwwali_get(domid, waiold)) lreturn(err_read);
  
  if( pg->Exec("BEGIN") != PGRES_COMMAND_OK ) lreturn(err_write);
  wwwali_info wai(w);
  wai.prefix = lower(wai.prefix);
  wai.dir = dir_clear(wai.dir);
  if( pg->Exec(("UPDATE domains_prefixes SET prefix='"
      +escape(wai.prefix)+"',"
      "dir='"+escape(wai.dir)+"' WHERE id='"
      +escape(wai.id)+"' AND domain_id='"+escape(domid)+"'").c_str()) 
      == PGRES_COMMAND_OK ) {
    if( ! pg->CmdTuples() ) {
      pg->Exec("ABORT");
      lreturn(err_noent);
    }
    // calls function writing info about domains to file which is processed
    // to create dns configuration
    if( pg->Exec("SELECT domains_prefixes_dump()") != PGRES_TUPLES_OK ) {
      pg->Exec("ABORT");
      lreturn(err_write);
    }
    if(!wwwali_exe(domid, wai)) {
      pg->Exec("ABORT");
      lreturn(err_write);
    }
    if(pg->Exec("COMMIT") != PGRES_COMMAND_OK ) {
      lreturn(err_write);
    }
    wwwali_exe(domid, waiold, 'r');
    lreturn(err_no);
  }
  pg->Exec("ABORT");
  lreturn(err_write);
}

/**
 *
 */
uint8_t cpginfodb::wwwali_rm(const string &domid, const string &id) {
  if( ! wwwali_exe(domid, id, 'r') ) {
    lreturn(err_write);
  }
  if( pg->Exec(("DELETE FROM domains_prefixes WHERE id='"+
      escape(id)+"' AND domain_id='"+escape(domid)+"'").c_str()) 
      == PGRES_COMMAND_OK ) {
    if( ! pg->CmdTuples() ) lreturn(err_noent);
    // calls function writing info about domains to file which is processed
    // to create dns configuration
    if( pg->Exec("SELECT domains_prefixes_dump()") != PGRES_TUPLES_OK ) {
      pg->Exec("ABORT");
      lreturn(err_write);
    }
    lreturn(err_no);
  }
  lreturn(err_write);
}

/**
 *
 */
uint8_t cpginfodb::wwwali_add(const string &domid, wwwali_info &wai) {
  wai.prefix = lower(wai.prefix);
  if( pg->Exec("SELECT nextval('domains_prefixes_id_seq')") == PGRES_TUPLES_OK
      && pg->Tuples() == 1 ) {
    const char *ptr = pg->GetValue(0,0);
    if(!ptr) lreturn(err_read);
    wai.id = ptr;
  } else lreturn(err_read);
  
  if( pg->Exec("BEGIN") != PGRES_COMMAND_OK ) lreturn(err_write);
  if( pg->Exec(("INSERT INTO domains_prefixes (id,prefix,dir,domain_id)"
      " VALUES('"+escape(wai.id)
      + "','"+escape(wai.prefix)+"','"+escape(dir_clear(wai.dir))
      +"','"+domid+"')").c_str()) == PGRES_COMMAND_OK && pg->CmdTuples() == 1 ) {
    // calls function writing info about domains to file which is processed
    // to create dns configuration
    if( pg->Exec("SELECT domains_prefixes_dump()") != PGRES_TUPLES_OK ) {
      pg->Exec("ABORT");
      lreturn(err_write);
    }
    if( ! wwwali_exe(domid, wai) ) {
      pg->Exec("ABORT");
      lreturn(err_write);
    }
    if(pg->Exec("COMMIT") != PGRES_COMMAND_OK ) lreturn(err_write);
    lreturn(err_no);
  }
  pg->Exec("ABORT");
  lreturn(err_write);
}

/**
 *
 */
bool cpginfodb::wwwali_exe(const string &domid, const string &id, char act ) {
  wwwali_info wai;
  wai.id = id;
  if(wwwali_get(domid, wai)) return false;
  return wwwali_exe(domid, wai, act);
}
/**
 *
 */
bool cpginfodb::wwwali_exe(const string &domid, const wwwali_info &wai, char act ) {
  char *av[5];
  int r, wstat, pid;
  string domain, rdomain;
  if( wwwali_dom_get(domid, domain)
      || wwwali_rdom_get(domid, rdomain) ) return false;
  string alias(conf_ftp_base+'/'+rdomain+"/www/"+wai.prefix+'.'+domain);
  string dir(conf_ftp_base+'/'+rdomain+"/www/"+rdomain+'/'+wai.dir);
  
  av[0] = (char*)conf_wwwali_exe.c_str();
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
uint8_t cpginfodb::wwwali_get(const string &domid, wwwali_info & wai ) {
  if( pg->Exec(("SELECT prefix,dir FROM domains_prefixes WHERE domain_id='"
      +escape(domid)+"' AND id='"+escape(wai.id)+"'").c_str()) == PGRES_TUPLES_OK ) {
    const char *ptr;
    if( ! pg->Tuples() ) lreturn(err_noent);
      ptr = pg->GetValue(0, 0);
      if(!ptr || ! *ptr) lreturn(err_noent);
      wai.prefix = ptr;
      ptr = pg->GetValue(0, 1);
      if(!ptr || ! *ptr) lreturn(err_noent);
      wai.dir = ptr;
      lreturn(err_no);
  }
  lreturn(err_read);
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
  edir = escape(dir_clear(edir));

  wais.clear();
  if( pg->Exec(("SELECT id,prefix,dir FROM domains_prefixes WHERE domain_id='"
      +escape(id)+"' AND (dir='"+edir+"' OR dir='/'||'" + edir 
      +"' OR dir='/'|| '"+edir+"' ||'/' OR dir='"
      +edir+"'||'/') ORDER BY prefix").c_str()) == PGRES_TUPLES_OK ) {
    vector<wwwali_info>::size_type i, cnt;
    wwwali_info wai;
    const char *ptr;
    if( ! (cnt=pg->Tuples()) ) lreturn(err_no);
    for( i=0; i<cnt; i++ ) {
      ptr = pg->GetValue(i, 0);
      if(!ptr) continue;
      wai.id = ptr;
      ptr = pg->GetValue(i, 1);
      if(!ptr || ! *ptr) continue;
      wai.prefix = ptr;
      ptr = pg->GetValue(i, 2);
      if(!ptr || ! *ptr) continue;
      wai.dir = ptr;
      wais.push_back(wai);
    }
    lreturn(err_no);
  }
  lreturn(err_read);
}

/**
 *
 */
uint8_t cpginfodb::wwwali_dom_get(const string &domid, string &domain ) {
  if( pg->Exec(("SELECT name FROM domains_all WHERE id='"
      +escape(domid)+"'").c_str()) == PGRES_TUPLES_OK ) {
    if( ! pg->Tuples() ) lreturn(err_noent);
    const char *ptr = pg->GetValue(0,0);
    if(! ptr || ! *ptr) lreturn(err_noent);
    domain = ptr;
    lreturn(err_no);
  }
  lreturn(err_read);
}

/**
 *
 */
uint8_t cpginfodb::wwwali_dom_id_get(const string &domain, string &id ) {
  if( pg->Exec(("SELECT id FROM domains_all WHERE name='"
      +escape(lower(domain))+"'").c_str()) == PGRES_TUPLES_OK ) {
    if( ! pg->Tuples() ) lreturn(err_noent);
    const char *ptr = pg->GetValue(0,0);
    if(! ptr || ! *ptr) lreturn(err_noent);
    id = ptr;
    lreturn(err_no);
  }
  lreturn(err_read);
}




/**
 * gets main domain name which has alias with given id (from domains_all)
 * \param domid id. from domains_all
 * \param domain main domain name from domains
 */
uint8_t cpginfodb::wwwali_rdom_get(const string &domid, string &domain ) {
  if( pg->Exec(("SELECT d.domain FROM domains as d,domains_all"
     " as dw where dw.id='"+escape(domid)
     +"' and dw.domain_id=d.id").c_str()) == PGRES_TUPLES_OK ) {
    if( ! pg->Tuples() ) lreturn(err_noent);
    const char *ptr = pg->GetValue(0,0);
    if(! ptr || ! *ptr) lreturn(err_noent);
    domain = ptr;
    lreturn(err_no);
  }
  lreturn(err_read);
}
/**
 *
 */
uint8_t cpginfodb::user_ls(const string &d, vector<user_info> &users) throw() {
  string dom(d);
  str2tb(dom);
  users.clear();
  if( pg->Exec(("SELECT id,login,date_crt,date_exp FROM "
          +dom+" WHERE login!='' AND login IS NOT NULL"
          " AND flags & 20 = 0 ORDER BY login").c_str()) == PGRES_TUPLES_OK ) {
    int tup = pg->Tuples();
    if(!tup) lreturn(err_no);
    users.reserve(tup);
    user_info fi;
    const char *ptr;
    for( int i=0; i < tup; ++i ) {
      ptr = pg->GetValue(i,0);
      if(!ptr) continue;
      fi.id = ptr;
      ptr = pg->GetValue(i,1);
      if(!ptr) continue;
      fi.user = ptr; 
      fi.date_crt = (ptr=pg->GetValue(i, 2)) ? ptr : "";
      fi.date_exp = (ptr=pg->GetValue(i, 3)) ? ptr : "";
      users.push_back(fi);
    }
    lreturn(err_no);
  }
  lreturn(err_read);
}
/**
 * gets user's id from special table including some extra info (client
 * information - address, phone, etc.)
 */
uint8_t cpginfodb::ui_id_get(const string &dom, const string &user, string &id) {
    if( pg->Exec(("SELECT id FROM users_info WHERE login='"
        +escape(lower(user))+"' AND domain_id="
        "(SELECT id FROM domains WHERE domain='"
        +escape(lower(dom))+"' LIMIT 1)").c_str()) == PGRES_TUPLES_OK ) {
            if( ! pg->Tuples() ) lreturn(err_noent);
            const char *tmp = pg->GetValue(0,0);
            id = ! tmp ? "" : tmp;
            lreturn(err_no);
    }
    lreturn(err_read);
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
