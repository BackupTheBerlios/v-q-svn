/**
 * little daemon waiting for files to change, and create other files
 * based on information from files
 *
 * at this time it creates tinydns configuration
 */ 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <cerrno>
#include <stdio.h>
#include <deque>
#include <utility>
#include <ext/stdio_filebuf.h>

#include "conf_tdns_uid.h"
#include "conf_tdns_gid.h"
#include "conf_tdns_root.h"
#include "conf_tdns_mode.h"
#include "conf_tdns_make.h"

#include "conf_dnsc_uid.h"
#include "conf_dnsc_gid.h"
#include "conf_dnsc_root.h"
#include "conf_dnsc_mode.h"
#include "conf_dnsc_restart.h"

#include "lock.h"

using namespace std;

void tdns_chg();

//
const char *me = NULL;
const char *tdns[] = {"domains", "domains_all", "domains_prefixes", "nameservers"};

struct _file {
  const char *name;
  bool chgd;
  struct stat st;
} files[] = {
  { tdns[0] },
  { tdns[1] },
  { tdns[2] },
  { tdns[3] }
};

struct {
  bool tdns;
} chgd;

/*
 *
 */
bool tdns_rev(const string &ip, string &rev) {
  string dot, dot1, dot2, dot3;
  string::size_type beg=0, dpos;

  dpos = ip.find('.', beg);
  if( dpos == string::npos ) return false;
  dot = ip.substr(beg, dpos-beg);
  beg = dpos+1;
  
  dpos = ip.find('.', beg);
  if( dpos == string::npos ) return false;
  dot1 = ip.substr(beg, dpos-beg);
  beg = dpos+1;

  dpos = ip.find('.', beg);
  if( dpos == string::npos ) return false;
  dot2 = ip.substr(beg, dpos-beg);
  beg = dpos+1;
  
  dot3 = ip.substr(beg, dpos-beg);

  rev = dot3+'.'+dot2+'.'+dot1+'.'+dot+".in-addr.arpa";
  return true;
}

/*
 *
 */
bool dnsc_chg(const string &host, const string &ns_ip) {
  string hf = conf_dnsc_root+"/servers/"+host;
  string hftmp = hf+".tmp";
  ofstream out( hftmp.c_str(), ios::trunc );
  if( ! out ) {
    cerr<<me<<": can't open: "<<hftmp<<": "<<strerror(errno)<<endl;
    return false;
  }
  if( chown( hftmp.c_str(), conf_dnsc_uid_i, conf_dnsc_gid_i) ) {
    cerr<<me<<": can't chown: "<<hftmp<<": "<<strerror(errno)<<endl;
    return false;
  }
  if( chmod( hftmp.c_str(), conf_dnsc_mode_i) ) {
    cerr<<me<<": can't chmod: "<<hftmp<<": "<<strerror(errno)<<endl;
    return false;
  }
  out<<ns_ip<<endl;
  out.flush();
  out.close();
  if(! out) return false;
  if( rename(hftmp.c_str(), hf.c_str()) ) {
    cerr<<me<<": rename to: "<<hf<<": "<<strerror(errno)<<endl;
    return false;
  }
  return true;
}
/*
 *
 */
void tdns_chg() {
  ifstream hosts(tdns[0]), alis(tdns[1]), alis1(tdns[2]), servers(tdns[3]);
  if(!hosts) {
    cerr<<me<<": can't open: "<<tdns[0]<<": "<<strerror(errno)<<endl;
    return;
  }
  if(!alis) {
    cerr<<me<<": can't open: "<<tdns[1]<<": "<<strerror(errno)<<endl;
    return;
  }
  if(!alis1) {
    cerr<<me<<": can't open: "<<tdns[2]<<": "<<strerror(errno)<<endl;
    return;
  }
  if(!servers) {
    cerr<<me<<": can't open: "<<tdns[3]<<": "<<strerror(errno)<<endl;
    return;
  }

  string datatmp = conf_tdns_root+"/data.pg2fs.tmp";
  ofstream out(datatmp.c_str(), ios::trunc );
  if( ! out ) {
    cerr<<me<<": can't open: "<< datatmp <<": "<<strerror(errno)<<endl;
    return;
  }
  if( chown(datatmp.c_str(), conf_tdns_uid_i, conf_tdns_gid_i) ) {
    cerr<<me<<": can't chown: "<< datatmp <<": "<<strerror(errno)<<endl;
    return;
  }
  if( chmod(datatmp.c_str(), conf_tdns_mode_i) ) {
    cerr<<me<<": can't chmod: "<< datatmp <<": "<<strerror(errno)<<endl;
    return;
  }
  deque< pair<string, string> > nss;
  deque< pair<string, string> >::const_iterator ns_cur, nss_end;
  pair< string, string > ns;
  string ln, host, ip, rev;
  string::size_type sppos;
  while(getline(servers, ln)) {
    sppos = ln.find(' ');
    host = ln.substr(0, sppos);
    ip = ln.substr(sppos+1);
    if(sppos == string::npos || host.empty() || ip.empty() ) {
      cerr<<me<<": "<<tdns[3]<<": bad line: "<<ln<<endl;
      continue;
    }
    ns.first = host;
    ns.second = ip;
    nss.push_back(ns);
  }
  nss_end = nss.end();

  while(getline(hosts, ln)) {
    sppos = ln.find(' ');
    host = ln.substr(0, sppos);
    ip = ln.substr(sppos+1);
    if(sppos == string::npos || host.empty() || ip.empty() 
      || ! tdns_rev(ip, rev)) {
      cerr<<me<<": "<<tdns[0]<<": bad line: "<<ln<<endl;
      continue;
    }
    if( ! dnsc_chg(host, nss[0].second) ) return;
    if( ! dnsc_chg(rev, nss[0].second) ) return;
    
    for( ns_cur = nss.begin(); ns_cur != nss_end; ++ ns_cur ) {
      out<<'.'<<host<<':'<<ns_cur->second<<':'<<ns_cur->first<<":259200"<<endl;
      out<<'.'<<rev<<':'<<ns_cur->second<<':'<<ns_cur->first<<":259200"<<endl;
    }
    out<<'='<<host<<':'<<ip<<":86400"<<endl;
  }
  if( hosts.bad() ) {
    cerr<<me<<": read error: "<<tdns[0]<<": "<<strerror(errno);
    return;
  }
  if( ! out ) {
    cerr<<me<<": write error: "<<datatmp<<": "<<strerror(errno);
    return;
  }
  
  while(getline(alis, ln)) {
    sppos = ln.find(' ');
    host = ln.substr(0, sppos);
    ip = ln.substr(sppos+1);
    if(sppos == string::npos || host.empty() || ip.empty() ) {
      cerr<<me<<": "<<tdns[0]<<": bad line: "<<ln<<endl;
      continue;
    }
    if( ! dnsc_chg(host, nss[0].second) ) return;
    
    for( ns_cur = nss.begin(); ns_cur != nss_end; ++ ns_cur ) {
      out<<'.'<<host<<':'<<ns_cur->second<<':'<<ns_cur->first<<":259200"<<endl;
    }
    out<<'+'<<host<<':'<<ip<<":86400"<<endl;
  }
  if( alis.bad() ) {
    cerr<<me<<": read error: "<<tdns[1]<<": "<<strerror(errno);
    return;
  }
  if( ! out ) {
    cerr<<me<<": write error: "<<datatmp<<": "<<strerror(errno);
    return;
  }
  
  while(getline(alis1, ln)) {
    sppos = ln.find(' ');
    host = ln.substr(0, sppos);
    ip = ln.substr(sppos+1);
    if(sppos == string::npos || host.empty() || ip.empty() ) {
      cerr<<me<<": "<<tdns[0]<<": bad line: "<<ln<<endl;
      continue;
    }
    out<<'+'<<host<<':'<<ip<<":86400"<<endl;
  }
  if( alis1.bad() ) {
    cerr<<me<<": read error: "<<tdns[1]<<": "<<strerror(errno);
    return;
  }
  out.flush();
  out.close();
  if( ! out ) {
    cerr<<me<<": write error: "<<datatmp<<": "<<strerror(errno);
    return;
  }
  string data = conf_tdns_root+"/data.pg2fs";
  if( rename(datatmp.c_str(), data.c_str()) ) {
    cerr<<me<<": rename to: "<< data <<": "<<strerror(errno);
    return;
  }

  int ret;
  ret = system(conf_tdns_make.c_str());
  if( ret == -1 ) {
    cerr<<me<<": system==-1: "<<strerror(errno)<<endl;
    return;
  } else if( WIFEXITED(ret) && WEXITSTATUS(ret) ) {
    cerr<<me<<": cmd: "<<conf_tdns_make<<": returned: "<<WEXITSTATUS(ret)<<endl;
  } else if( WIFSIGNALED(ret) ) {
    cerr<<me<<": cmd: "<<conf_tdns_make<<": signal: "<<WTERMSIG(ret)<<endl;
    return;
  }
  ret = system(conf_dnsc_restart.c_str());
  if( ret == -1 ) {
    cerr<<me<<": system==-1: "<<strerror(errno)<<endl;
    return;
  } else if( WIFEXITED(ret) && WEXITSTATUS(ret) ) {
    cerr<<me<<": cmd: "<<conf_dnsc_restart<<": returned: "<<WEXITSTATUS(ret)<<endl;
  } else if( WIFSIGNALED(ret) ) {
    cerr<<me<<": cmd: "<<conf_dnsc_restart<<": signal: "<<WTERMSIG(ret)<<endl;
    return;
  }
}
/*
 *
 */
void setup() {
  unsigned files_cnt = sizeof(files)/sizeof(*files);
  if( files_cnt-- ) {
    do {
      if( stat(files[files_cnt].name, & files[files_cnt].st ) ) {
        cerr<<me<<": stat: "<<files[files_cnt].name<<": "<<strerror(errno)<<endl;
        exit(111);
      }
      files[files_cnt].chgd = false;
    } while(files_cnt--);
  } else {
    cerr<<me<<": no files"<<endl;
    exit(111);
  }
}

/*
 *
 */
void chk_chgd(unsigned pos) {
  unsigned cnt = sizeof(tdns)/sizeof(*tdns);
  if(cnt--) {
    do {
      if( ! strcmp(files[pos].name, tdns[cnt]) ) {
        chgd.tdns = true;
      }
    } while(cnt--);
  }
}
/*
 *
 */
void run() {
  unsigned cnt;
  struct stat st;
  for(;;) {
    sleep(1);
    cnt = sizeof(files)/sizeof(*files)-1;
    memset( & chgd, 0, sizeof(chgd) );

    do {
      if( ! stat(files[cnt].name, &st) ) {
        if( st.st_mtime != files[cnt].st.st_mtime ) {
          cout<<me<<": file was changed: "<<files[cnt].name<<endl;
          files[cnt].st = st;
          files[cnt].chgd = true;
          chk_chgd(cnt);
        }
      } else cerr<<me<<": stat: "<<files[cnt].name<<strerror(errno)<<endl;
    } while(cnt--);
    if( chgd.tdns ) tdns_chg();
  }
}
/*
 *
 */
int main(int ac, char **av) {
  me = *av;
  if(ac !=2) {
    cerr<<*av<<": specify a directory"<<endl;
    return 111;
  }
  umask(0);
  if(chdir(av[1])) {
    cerr<<*av<<": chdir: "<<av[1]<<": "<<strerror(errno)<<endl;
    return 111;
  }
  ofstream lock("lock", ios::trunc);
  if( chmod("lock", 0600) ) {
    cerr<<*av<<": chmod: lock: "<<strerror(errno)<<endl;
    return 111;
  }
  __gnu_cxx::stdio_filebuf<char> fb = (__gnu_cxx::stdio_filebuf<char>) *lock.rdbuf();
  if( lock_exnb(lock.rdbuf()->_fileno) ) {
    if(errno == EWOULDBLOCK) {
      cerr<<*av<<": already locked"<<endl;
    } else cerr<<*av<<": can't lock: "<<strerror(errno)<<endl;
    return 111;
  }
  setup();
  run();
  // never reached
  return 0;
}
