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
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <algorithm>
#include <dirent.h>
#include <signal.h>
#include <iomanip>
#include <fstream>

#include "conf_qmail.h"
#include "conf_qmail_mode.h"
#include "conf_vq.h"
#include "conf_vq_mode.h"
#include "conf_vq_fmode.h"
#include "conf_vq_uid.h"
#include "conf_vq_gid.h"
#include "conf_vq_user.h"
#include "cfsvq.h"
#include "lower.h"
#include "lock.h"
#include "dirs.h"
#include "split.h"
#include "util.h"
#include "d_namlen.h"
#include "sys.h"

using namespace std;

const char cfsvq::tmp_end[] = ".tmp"; //!< prefix for temporary files

char cfsvq::ud_sup[] = "FSM"; //!< F - forwarding, S - sms notification, M - maildir

#define lreturn(x) do { uint8_t x1=(x); if(x1) lastret = x1; return x1; } while(0)
/**
 * \param a for some operation authorization module is not used so it may be set to NULL, if it's NULL and code wants to use it logic_error will be thrown 
 */
cfsvq::cfsvq(cauth *a)
{
    auth = a;
    umask(0);
    lastret = 0;
}

/**
\return unique value which will be used to create temporary files
*/
string cfsvq::uniq() throw(runtime_error)
{
    ostringstream u;
    struct timeval tv;
    if(gettimeofday(&tv,NULL)==-1)
            throw runtime_error("cfsvq::uniq - gettimeofday");
    u<<tv.tv_sec<<tv.tv_usec<<'.'<<getpid();
    return u.str();
}

/**
*/
char cfsvq::dom_alias_rm( const string & d ) throw()
{
    char ret;
    string dom(lower(d));
    if( (ret=assign_ex(dom)) == (char) 0xff ) lreturn(err_dom_ex);
    if( ret ) lreturn(ret);
    if( virt_rm(dom)
        || rename(tmpfs.virt_rm.c_str(), 
                (conf_qmail+"/control/virtualdomains").c_str()) ) lreturn(err_temp);
    lreturn(0);
}
/**
@param domain name
@return 0 on success
@description adds domain to assign file, it creates temporary file 
(name in tmpfs.assign_add) which you should rename to assign on success
*/
char cfsvq::assign_add( const string &dom ) throw()
{   
    string fn = conf_qmail+"/users/assign";
    fstream l((fn+".lock").c_str(),ios::in);
    if( ! l ) {
            l.open((fn+".lock").c_str(),ios::in|ios::out);
            if( ! l ) lreturn(err_temp);
    }
    if( lock_exnb(l.rdbuf()->fd()) ) lreturn(err_temp);
    fstream a(fn.c_str(),ios::in|ios::nocreate);
    if(!a) {
            a.open(fn.c_str(),ios::in|ios::out,conf_qmail_mode_i);
            if(!a) lreturn(err_temp);
    }
    string at_fn = fn+uniq();
    ofstream at(at_fn.c_str(),ios::in,0600);
    if(!at) lreturn(err_temp);
    tmpfs.assign_add = at_fn;
    
    string ln;
    while(getline(a,ln)) {
            if(ln==".") break;
            if(ln.substr(0,dom.length()+1)==dom+"-") {
                    unlink(at_fn.c_str());
                    lreturn(err_dom_ex);
            }
            at << ln << endl;
            if(! at) {
                    unlink(at_fn.c_str());
                    lreturn(err_temp); 
            }
    }
    at << '+'<< dom << "-:" << conf_vq_user << ':' 
            << conf_vq_uid_s << ':' << conf_vq_gid_s << ':'
            << conf_vq << "/domains/" << split_dom(dom)
            << '/' << dom << ":-::" << endl << '.' << endl;
    at.flush();
    if(a.bad() || !at) {
            unlink(at_fn.c_str());
            lreturn(err_temp);
    }
    struct stat st;
    if(!fstat(a.rdbuf()->fd(), &st)) {
            fchown(at.rdbuf()->fd(), st.st_uid, st.st_gid); 
            fchmod(at.rdbuf()->fd(), st.st_mode & 07777 );
    }
    lreturn(0);
}

/**
@description checks whether domain is in assign, if it's not returns 0, if it is
returns 0xff
*/
char cfsvq::assign_ex( const string &dom ) throw()
{   
    string fn = conf_qmail+"/users/assign";
    fstream a(fn.c_str(),ios::in|ios::nocreate);
    if(!a) lreturn(err_temp);
    
    string ln, domext = "+"+dom+"-";
    string::size_type domextl = domext.length()+1;
    while(getline(a,ln)) {
            if(ln==".") break;
            if(ln.substr(0,domextl)==domext) lreturn(0xff);
    }
    if( a.bad() ) {
            lreturn(err_temp);
    }
    lreturn(0);
}
/**
@param domain name
@return 0 on success
@description adds domain to assign file, it creates temporary file 
(name in tmpfs.assign_add) which you should rename to assign on success
*/
char cfsvq::assign_rm( const string &dom ) throw()
{   
    string fn = conf_qmail+"/users/assign";
    fstream l((fn+".lock").c_str(),ios::in);
    if( ! l ) {
            l.open((fn+".lock").c_str(),ios::in|ios::out,0666);
            if( ! l ) lreturn(err_temp);
    }
    if( lock_exnb(l.rdbuf()->fd()) ) lreturn(err_temp);
    fstream a(fn.c_str(),ios::in|ios::nocreate);
    if(!a) {
            a.open(fn.c_str(),ios::in|ios::out,conf_qmail_mode_i);
            if(!a) lreturn(err_temp);
    }
    string at_fn = fn+uniq();
    ofstream at(at_fn.c_str(),ios::in,0600);
    if(!at) lreturn(err_temp);
    tmpfs.assign_rm = at_fn;
    
    string ln, domext = "+"+dom+"-";
    while(getline(a,ln)) {
            if(ln==".") break;
            if(ln.substr(0,domext.length())==domext) continue;
            at << ln << endl;
            if(! at) {
                    unlink(at_fn.c_str());
                    lreturn(err_temp); 
            }
    }
    at << '.' << endl;
    at.flush();
    if(a.bad() || !at) {
            unlink(at_fn.c_str());
            lreturn(err_temp);
    }
    struct stat st;
    if(!fstat(a.rdbuf()->fd(), &st)) {
            fchown(at.rdbuf()->fd(), st.st_uid, st.st_gid); 
            fchmod(at.rdbuf()->fd(), st.st_mode & 07777 );
    }
    lreturn(0);
}

/**
*/
void cfsvq::dom_add_clean(const string &dom) throw()
{
    assert_auth();
    auth->dom_rm(dom);
    rmdirrec(tmpfs.dom_add_dir);
    if( ! tmpfs.locals_rm.empty() )
            unlink(tmpfs.locals_rm.c_str());
    if( ! tmpfs.rcpt_add.empty() )
            unlink(tmpfs.rcpt_add.c_str());
    if( ! tmpfs.morercpt_add.empty() )
            unlink(tmpfs.morercpt_add.c_str());
    if( ! tmpfs.virt_add.empty() )
            unlink(tmpfs.virt_add.c_str());
    if( ! tmpfs.assign_add.empty() )
            unlink(tmpfs.assign_add.c_str());
}

/**
@description removes domain
*/
char cfsvq::dom_rm(const string &d) throw()
{
    assert_auth();
    string dom(lower(d));
    bool partial = false;
    if(rcpt_rm(dom)
       || rename(tmpfs.rcpt_rm.c_str(), 
               (conf_qmail+"/control/rcpthosts").c_str())
       || morercpt_rm(dom)
       || rename(tmpfs.morercpt_rm.c_str(), 
               (conf_qmail+"/control/morercpthosts").c_str()) ) lreturn(err_temp);
    morercpt_comp();
    if(!auth->dom_rm(dom)) partial = true;
    if(virt_rm(dom) 
       || rename(tmpfs.virt_rm.c_str(), 
               (conf_qmail+"/control/virtualdomains").c_str()) ) partial = true;
    if(assign_rm(dom)
       || rename(tmpfs.assign_rm.c_str(),
               (conf_qmail+"/users/assign").c_str()) ) partial = true;
    assign_comp();
    skillall("qmail-send", SIGHUP);
    if(!rmdirrec(conf_vq+"/domains/"+split_dom(dom)+'/'+dom)) partial = true;
    lreturn(partial ? err_dom_rm_partial : 0);
}
/**
@description adds domain to system
@param dom valid domain name
*/
char cfsvq::dom_add(const string &d) throw()
{
    assert_auth();
    string dom(lower(d));
    if(!dom_val(dom)) lreturn(err_dom_inv);
    tmpfs.dom_add_dir = conf_vq+"/domains/"+split_dom(dom)+'/'+dom;
    if(!mkdirhier((tmpfs.dom_add_dir).c_str(), 
        conf_vq_mode_i, conf_vq_uid_i, conf_vq_gid_i)) lreturn(err_dom_ex);
    /* dot file with a call to deliver */
    string dotfile = tmpfs.dom_add_dir+"/.qmail-default";
    if(!dumpstring( dotfile, (string)"| "+conf_vq+"/bin/deliver\n", conf_vq_fmode_i)
       || chown(dotfile.c_str(), conf_vq_uid_i, conf_vq_gid_i)) {
            rmdirrec(tmpfs.dom_add_dir); 
            lreturn(err_dom_dot);
    }
            
    if(!auth->dom_add(dom)) { 
            rmdirrec(tmpfs.dom_add_dir); 
            lreturn(err_auth_dom_add); 
    }
    if(locals_rm(dom)) {
            dom_add_clean(dom);
            lreturn(err_locals_rm);
    }
    if(rcpt_add(dom)) {
            dom_add_clean(dom);
            lreturn(err_rcpt_add);
    }
    if(virt_add(dom,dom)) {
            dom_add_clean(dom);
            lreturn(err_virt_add); 
    }
    switch(assign_add(dom)) {
    case err_temp: 
            dom_add_clean(dom);
            lreturn(err_assign_add);
    case err_dom_ex: 
            dom_add_clean(dom);
            lreturn(err_dom_ex);
    }
    if(rename(tmpfs.locals_rm.c_str(),(conf_qmail+"/control/locals").c_str())) {
            dom_add_clean(dom);
            lreturn(err_locals_rm);
    }
    if(tmpfs.rcpt_add.empty()) {
            if(rename(tmpfs.morercpt_add.c_str(),(conf_qmail+"/control/morercpthosts").c_str())) {
                    dom_add_clean(dom);
                    lreturn(err_rcpt_add);
            }
            morercpt_comp();
    } else {
            if(rename(tmpfs.rcpt_add.c_str(),(conf_qmail+"/control/rcpthosts").c_str())) {
                    dom_add_clean(dom);
                    lreturn(err_rcpt_add);
            }
    }
    if(rename(tmpfs.virt_add.c_str(),(conf_qmail+"/control/virtualdomains").c_str())) {
            dom_add_clean(dom);
            lreturn(err_virt_add);
    }
    if(rename(tmpfs.assign_add.c_str(),(conf_qmail+"/users/assign").c_str())) {
            dom_add_clean(dom);
            lreturn(err_assign_add);
    }
    if(assign_comp()) {
            lreturn(err_assign_comp);
    }
    skillall("qmail-send", SIGHUP);
    lreturn(0);
}

/**
@description adds host to rcpthosts, it creates temp. file 
(name in tmpfs.rcpt_add) which you should rename to rcpthosts on success,
if morercpthosts is changed rcpthosts is set to "", tmpfs.morercpt_add is set
*/
char cfsvq::rcpt_add(const string &dom) throw()
{
    string r_fn = conf_qmail+"/control/rcpthosts", rt_fn = r_fn+uniq();
    fstream lock((r_fn+".lock").c_str(),ios::in);
    if( ! lock ) {
            lock.open((r_fn+".lock").c_str(),ios::in|ios::out,0666);
            if( ! lock ) lreturn(err_temp);
    }
    if( lock_exnb(lock.rdbuf()->fd()) ) lreturn(err_temp);

    fstream r(r_fn.c_str(),ios::in);
    if(!r) {
            r.open(r_fn.c_str(),ios::out|ios::in,conf_qmail_mode_i);
            if(!r) lreturn(err_temp);
    }
    ofstream rt(rt_fn.c_str(),ios::out|ios::trunc,0600);
    if(!rt) lreturn(err_temp);
    tmpfs.rcpt_add = rt_fn;

    string ln;
    uint32_t rcpt_cnt=0;
    for( ; getline(r,ln) && rcpt_cnt < 50; rcpt_cnt++ ) {
            if(ln==dom) continue;
            rt<<ln << '\n';
            if(!rt) {
                    unlink(rt_fn.c_str());
                    lreturn(err_temp);
            }
    }
    if(r.bad()) {
            unlink(rt_fn.c_str());
            lreturn(err_temp);
    }
    if( rcpt_cnt == 50 ) {
            unlink(rt_fn.c_str());
            tmpfs.rcpt_add = "";
            lreturn(morercpt_add(dom));
    }
    rt << dom << endl;
    rt.flush();
    if(!rt) {
            unlink(rt_fn.c_str());
            lreturn(err_temp);
    }
    struct stat st;
    if(!fstat(r.rdbuf()->fd(), &st)) {
            fchown(rt.rdbuf()->fd(), st.st_uid, st.st_gid); 
            fchmod(rt.rdbuf()->fd(), st.st_mode & 07777 );
    }
    lreturn(0);
}

/**
@description remove host to rcpthosts, it creates temp. file 
(name in tmpfs.rcpt_rm) which you should rename to rcpthosts on success
*/
char cfsvq::rcpt_rm(const string &dom) throw()
{
    string r_fn = conf_qmail+"/control/rcpthosts", rt_fn = r_fn+uniq();
    fstream lock((r_fn+".lock").c_str(),ios::in);
    if( ! lock ) {
            lock.open((r_fn+".lock").c_str(),ios::in|ios::out,0666);
            if( ! lock ) lreturn(err_temp);
    }
    if( lock_exnb(lock.rdbuf()->fd()) ) lreturn(err_temp);

    fstream r(r_fn.c_str(),ios::in);
    if(!r) {
            r.open(r_fn.c_str(),ios::out|ios::in,conf_qmail_mode_i);
            if(!r) lreturn(err_temp);
    }
    ofstream rt(rt_fn.c_str(),ios::out|ios::trunc,0600);
    if(!rt) lreturn(err_temp);
    tmpfs.rcpt_rm = rt_fn;
    
    string ln;
    uint32_t rcpt_cnt=0;
    for( ; getline(r,ln) && rcpt_cnt < 50; rcpt_cnt++ ) {
            if(ln==dom) continue;
            rt<<ln << '\n';
            if(!rt) {
                    unlink(rt_fn.c_str());
                    lreturn(err_temp);
            }
    }
    if(r.bad()) {
            unlink(rt_fn.c_str());
            lreturn(err_temp);
    }
    rt.flush();
    if(!rt) {
            unlink(rt_fn.c_str());
            lreturn(err_temp);
    }
    struct stat st;
    if(! fstat(r.rdbuf()->fd(), &st)) {
            fchown(rt.rdbuf()->fd(), st.st_uid, st.st_gid); 
            fchmod(rt.rdbuf()->fd(), st.st_mode & 07777 );
    }
    lreturn(0);
}

/**
@description removes domain from locals, sets tmpfs.locals_rm to
name of created file, on success you have to rename it to locals
*/
char cfsvq::locals_rm(const string &dom) throw()
{
    string l_fn = conf_qmail+"/control/locals", lt_fn = l_fn+uniq();
    fstream lock((l_fn+".lock").c_str(),ios::in);
    if( ! lock ) {
            lock.open((l_fn+".lock").c_str(),ios::in|ios::out,0666);
            if( ! lock ) lreturn(err_temp);
    }
    if( lock_exnb(lock.rdbuf()->fd()) ) lreturn(err_temp);

    fstream l(l_fn.c_str(),ios::in);
    if(!l) {
            l.open(l_fn.c_str(),ios::out|ios::in,conf_qmail_mode_i);
            if(!l) lreturn(err_temp);
    }
    ofstream lt(lt_fn.c_str(),ios::out|ios::trunc,0600);
    if(!lt) lreturn(err_temp);
    tmpfs.locals_rm = lt_fn;
    
    string ln;
    while( getline(l,ln) ) {
            if(ln==dom) continue;
            lt<<ln<< '\n';
            if(!lt) {
                    unlink(lt_fn.c_str());
                    lreturn(err_temp);
            }
    }
    if(l.bad()) {
            unlink(lt_fn.c_str());
            lreturn(err_temp);
    }
    lt.flush();
    if(!lt) {
            unlink(lt_fn.c_str());
            lreturn(err_temp);
    }
    struct stat st;
    if(!fstat(l.rdbuf()->fd(), &st)) {
            fchown(lt.rdbuf()->fd(), st.st_uid, st.st_gid); 
            fchmod(lt.rdbuf()->fd(), st.st_mode & 07777 );
    }
    lreturn(0);
}

/**
@description add domain to virtualdomains, it creates temp. file (name in
tmpfs.virt_add) which should be renamed to virtualdomains on success
*/
char cfsvq::virt_add(const string &dom,const string &al) throw()
{
    string v_fn = conf_qmail+"/control/virtualdomains", vt_fn = v_fn+uniq();
    fstream lock((v_fn+".lock").c_str(),ios::in);
    if( ! lock ) {
            lock.open((v_fn+".lock").c_str(),ios::in|ios::out,0666);
            if( ! lock ) lreturn(err_temp);
    }
    if( lock_exnb(lock.rdbuf()->fd()) ) lreturn(err_temp);

    fstream v(v_fn.c_str(),ios::in);
    if(!v) {
            v.open(v_fn.c_str(),ios::out|ios::in,conf_qmail_mode_i);
            if(!v) lreturn(err_temp);
    }
    ofstream vt(vt_fn.c_str(),ios::out|ios::trunc,0600);
    if(!vt) lreturn(err_temp);
    tmpfs.virt_add = vt_fn;
    
    string ln;
    while( getline(v,ln) ) {
            if(ln.substr(0,ln.find(':'))==dom) lreturn(err_dom_ex);
            vt<<ln<< '\n';
            if(!vt) {
                    unlink(vt_fn.c_str());
                    lreturn(err_temp);
            }
    }
    vt << dom << ':' << al << '\n';
    vt.flush();
    if(!vt || v.bad() ) {
            unlink(vt_fn.c_str());
            lreturn(err_temp);
    }
    struct stat st;
    if(!fstat(v.rdbuf()->fd(), &st)) {
            fchown(vt.rdbuf()->fd(), st.st_uid, st.st_gid); 
            fchmod(vt.rdbuf()->fd(), st.st_mode & 07777 );
    }
    lreturn(0);
}

/**
@description add domain to virtualdomains, it creates temp. file (name in
tmpfs.virt_add) which should be renamed to virtualdomains on success
*/
char cfsvq::virt_rm(const string &dom) throw()
{
    string v_fn = conf_qmail+"/control/virtualdomains", vt_fn = v_fn+uniq();
    fstream lock((v_fn+".lock").c_str(),ios::in);
    if( ! lock ) {
            lock.open((v_fn+".lock").c_str(),ios::in|ios::out,0666);
            if( ! lock ) lreturn(err_temp);
    }
    if( lock_exnb(lock.rdbuf()->fd()) ) lreturn(err_temp);

    fstream v(v_fn.c_str(),ios::in);
    if(!v) {
            v.open(v_fn.c_str(),ios::out|ios::in,conf_qmail_mode_i);
            if(!v) lreturn(err_temp);
    }
    ofstream vt(vt_fn.c_str(),ios::out|ios::trunc,0600);
    if(!vt) lreturn(err_temp);
    tmpfs.virt_rm = vt_fn;

    string ln;
    while( getline(v,ln) ) {
            if(ln.substr(0,ln.find(':'))==dom) continue;
            vt<<ln<< '\n';
            if(!vt) {
                    unlink(vt_fn.c_str());
                    lreturn(err_temp);
            }
    }
    vt.flush();
    if( !vt || v.bad() ) {
            unlink(vt_fn.c_str());
            lreturn(err_temp);
    }
    // try to preserve all of file's information, ignore errors
    struct stat st;
    if(!fstat(v.rdbuf()->fd(), &st)) {
            fchown(vt.rdbuf()->fd(), st.st_uid, st.st_gid); 
            fchmod(vt.rdbuf()->fd(), st.st_mode & 07777 );
    }
    lreturn(0);
}

/**
@description runs qmail-newmrh
*/
char cfsvq::morercpt_comp() throw()
{
    string qmrh;
    pid_t pid;
    char *qmrhe[2];
    switch((pid=vfork())) {
    case -1: return 2;
    case 0: qmrh = conf_qmail + "/bin/qmail-newmrh";
            qmrhe[0] = (char*)qmrh.c_str();
            qmrhe[1] = NULL;
            execv( *qmrhe, qmrhe );
            _exit(111);
    }
    while( wait(&pid) == -1 && errno == EINTR );
    return WIFEXITED(pid) && WEXITSTATUS(pid) ? 0 : 1;
}

/**
@description runs qmail-newu
*/
char cfsvq::assign_comp() throw()
{
    string qmrh;
    pid_t pid;
    char *qmrhe[2];
    switch((pid=vfork())) {
    case -1: return 2;
    case 0: qmrh = conf_qmail + "/bin/qmail-newu";
            qmrhe[0] = (char*)qmrh.c_str();
            qmrhe[1] = NULL;
            execv( *qmrhe, qmrhe );
            _exit(111);
    }
    while( wait(&pid) == -1 && errno == EINTR );
    return WIFEXITED(pid) && ! WEXITSTATUS(pid) ? 0 : 1;
}

/**
@description adds domain to morercpthosts, creates temp. file which should
be renamed to morercpthosts, you should also run morercpt_comp.
*/
char cfsvq::morercpt_add(const string &dom) throw()
{
    string m_fn = conf_qmail+"/control/morercpthosts", mt_fn = m_fn+uniq();
    fstream lock((m_fn+".lock").c_str(),ios::in);
    if( ! lock ) {
            lock.open((m_fn+".lock").c_str(),ios::in|ios::out,0666);
            if( ! lock ) lreturn(err_temp);
    }
    if( lock_exnb(lock.rdbuf()->fd()) ) lreturn(err_temp);

    fstream m(m_fn.c_str(),ios::in);
    if(!m) {
            m.open(m_fn.c_str(),ios::out|ios::in,conf_qmail_mode_i);
            if(!m) lreturn(err_temp);
    }
    ofstream mt(mt_fn.c_str(),ios::out|ios::trunc,0600);
    if(!mt) lreturn(err_temp);
    tmpfs.morercpt_add = mt_fn;
    
    string ln;
    while( getline(m,ln) ) {
            mt<<ln<< '\n';
            if(!mt) {
                    unlink(mt_fn.c_str());
                    lreturn(err_temp);
            }
    }
    mt<<dom << '\n';
    mt.flush();
    if(m.bad() || !mt) {
            unlink(mt_fn.c_str());
            lreturn(err_temp);
    }
    // try to preserve all of file's information, ignore errors
    struct stat st;
    if(!fstat(m.rdbuf()->fd(), &st)) {
            fchown(mt.rdbuf()->fd(), st.st_uid, st.st_gid);
            fchmod(mt.rdbuf()->fd(), st.st_mode & 07777 );
    }
    lreturn(0);
}

/**
@description adds domain to morercpthosts, creates temp. file which should
be renamed to morercpthosts, you should also run morercpt_comp.
*/
char cfsvq::morercpt_rm(const string &dom) throw()
{
    string m_fn = conf_qmail+"/control/morercpthosts", mt_fn = m_fn+uniq();
    fstream lock((m_fn+".lock").c_str(),ios::in);
    if( ! lock ) {
            lock.open((m_fn+".lock").c_str(),ios::in|ios::out,0666);
            if( ! lock ) lreturn(err_temp);
    }
    if( lock_exnb(lock.rdbuf()->fd()) ) lreturn(err_temp);

    fstream m(m_fn.c_str(),ios::in);
    if(!m) {
            m.open(m_fn.c_str(),ios::out|ios::in,conf_qmail_mode_i);
            if(!m) lreturn(err_temp);
    }
    ofstream mt(mt_fn.c_str(),ios::out|ios::trunc,0600);
    if(!mt) lreturn(err_temp);
    tmpfs.morercpt_rm = mt_fn;
    
    string ln;
    while( getline(m,ln) ) {
            if(ln==dom) continue;
            mt<<ln<< '\n';
            if(!mt) {
                    unlink(mt_fn.c_str());
                    lreturn(err_temp);
            }
    }
    mt.flush();
    if(m.bad() || !mt) {
            unlink(mt_fn.c_str());
            lreturn(err_temp);
    }
    // try to preserve all of file's information, ignore errors
    struct stat st;
    if(!fstat(m.rdbuf()->fd(), &st)) {
            fchown(mt.rdbuf()->fd(), st.st_uid, st.st_gid); 
            fchmod(mt.rdbuf()->fd(), st.st_mode & 07777 );
    }
    lreturn(0);
}

/*
 * \param u user's name
 * \param d domain's name
 * \param p password
 * \param flags if (flags & 1) != 0 then don't check whether login is banned
 */
char cfsvq::user_add( const string & u, const string &d, const string & p, 
                uint8_t flags ) throw()
{
    assert_auth();
    string user(lower(u)), dom(lower(d));

    string domdir = conf_vq+"/domains/"+split_dom(dom)+'/'+dom+'/';
    string spuser = split_user(user);
    string user_add_dir = domdir + spuser + '/'+ u;

    /* check wheter domain has default quota for users */
    quota_value qm=0;
    char ret;
    switch((ret=qt_def_get(dom, qm))) {
    case err_no: break;
    case err_qt_no: break;
    lreturn(ret);
    }

    if(!mkdirhier(user_add_dir.c_str(), conf_vq_mode_i,
            conf_vq_uid_i, conf_vq_gid_i)) lreturn(err_user_ex);
    if(maildirmake(user_add_dir+"/Maildir")) {
            rmdirrec(user_add_dir);
            lreturn(err_temp);
    }
    if( qm && (ret=qt_set(dom, user, qm)) != err_no ) {
            rmdirrec(user_add_dir);
            lreturn(ret);
    }
    if((lastret=auth->user_add(u,d,p,flags))) {
            rmdirrec(user_add_dir);
            return lastret;
    }
    string dotuser = dotfile(dom, user, "");
    if( ! dumpstring(dotuser, (string)"./"+user+"/Maildir/\n", conf_vq_fmode_i )
       || chown(dotuser.c_str(), conf_vq_uid_i, conf_vq_gid_i)) {
            auth->user_rm(user,dom);
            rmdirrec(user_add_dir);
            lreturn(err_temp);
    }
    lreturn(0);
}

/**
*/
char cfsvq::maildirmake(const string & d) throw()
{
    if( mkdir(d.c_str(), conf_vq_mode_i )
        || chown(d.c_str(), conf_vq_uid_i, conf_vq_gid_i)) return 1;
    string dir = d+"/new"; 
    if( mkdir(dir.c_str(), conf_vq_mode_i ) 
        || chown(dir.c_str(), conf_vq_uid_i, conf_vq_gid_i)) return 1;
    dir = d+"/cur";
    if( mkdir(dir.c_str(), conf_vq_mode_i ) 
        || chown(dir.c_str(), conf_vq_uid_i, conf_vq_gid_i)) return 1;
    dir = d+"/tmp";
    if( mkdir(dir.c_str(), conf_vq_mode_i ) 
        || chown(dir.c_str(), conf_vq_uid_i, conf_vq_gid_i)) return 1;
    return 0;
}

/**
*/
char cfsvq::user_rm(const string &u,const string &d) throw()
{
    assert_auth();
    
    string user(lower(u)), dom(lower(d));

    if( ! auth->user_rm(u,d) ) lreturn(err_auth_user_rm);
    string dir = conf_vq+"/domains/"+split_dom(dom)+'/'+dom+'/'
            +split_user(user) + '/';
    
    ostringstream dir_mv;
    struct timeval time_mv;
    gettimeofday(&time_mv, NULL);
    dir_mv<<conf_vq<<"/deleted/@"<<time_mv.tv_sec
            <<'.'<<time_mv.tv_usec<<'.'<<user<<'@'<<dom;
    if(rename((dir+user).c_str(), dir_mv.str().c_str())) 
            lreturn(err_ren);
    
    replace(user.begin(),user.end(),'.',':');
    DIR *dotdir = opendir(dir.c_str());
    if( dotdir ) {
            struct dirent *de;
            char * name;
            string::size_type userl = user.length(); // +6 = .qmail-
            while( (de=readdir(dotdir)) ) {
                    name = de->d_name;
                    if( _D_EXACT_NAMLEN(de) < 7
                      || _D_EXACT_NAMLEN(de)-7 < userl 
                      || *(name++) != '.' || *(name++) != 'q' 
                      || *(name++) != 'm'
                      || *(name++) != 'a' || *(name++) != 'i'
                      || *(name++) != 'l' || *(name++) != '-' ) continue;
                    if( name == user 
                        || ( name[userl] == '-'
                           && strncmp(name+userl,user.c_str(),userl) ) )
                            unlink((dir+de->d_name).c_str());
            }
            closedir(dotdir);
    } else lreturn(err_rd);
    
    lreturn(0);
}

/**
@return 1 on valid domain name, 0 otherwise
*/
char cfsvq::dom_val(const string & d) throw()
{
    const char *ptr = d.c_str();
    if( d.empty() ) return 0;
    for( ; *ptr; ptr++ ) {
            if( ! ( ( *ptr >= 'A' && *ptr <= 'Z' )
                || ( *ptr >= 'a' && *ptr <= 'z' )
                || ( *ptr >= '0' && *ptr <='9' )
                || *ptr == '.' || *ptr == '-' ) )
                    return 0;
    }
    return 1;
}

/**
@return 1 if user name is valid, 0 otherwise
*/
char cfsvq::user_val(const string &u) throw()
{
    if( u.empty() ) return 0;
    const char *ptr = u.c_str();
    for( ; *ptr; ptr++ ) {
            if( ! ( (*ptr >= 'a' && *ptr <= 'z')
                || (*ptr >= 'A' && *ptr <= 'Z' )
                || (*ptr >= '0' && *ptr <= '9' )
                || *ptr == '.' || *ptr == '`' || *ptr == '~'
                || *ptr == '!' || *ptr == '#'
                || *ptr == '$' || *ptr == '^' || *ptr == '&' 
                || *ptr == '*' || *ptr == ':'
                || *ptr == '_' || *ptr == '=' || *ptr == '+'
                || *ptr == '{' || *ptr == '}' || *ptr == '?' ) )
                    return 0;
    }
    return 1;
}

/**
*/
char cfsvq::dom_alias_add(const string &d, const string &a) throw()
{
    char ret;
    string dom(lower(d)), ali(lower(a));
    ret = virt_add(ali,dom);
    if(ret) {
            if(!tmpfs.virt_add.empty())
                    unlink(tmpfs.virt_add.c_str());
            lreturn(ret);
    }
    ret = rcpt_add(ali);
    if(ret) {
            if(!tmpfs.virt_add.empty()) unlink(tmpfs.virt_add.c_str());
            if(!tmpfs.rcpt_add.empty()) unlink(tmpfs.rcpt_add.c_str());
            if(!tmpfs.morercpt_add.empty()) unlink(tmpfs.morercpt_add.c_str());
            lreturn(ret);
    }
    if( !tmpfs.rcpt_add.empty() ) {
            if( rename(tmpfs.morercpt_add.c_str(), 
                        (conf_qmail+"/control/morercpthosts").c_str()))
                    lreturn(err_ren);
            morercpt_comp();
    } else if( rename(tmpfs.rcpt_add.c_str(), (conf_qmail+"/control/rcpthosts").c_str()))
            lreturn(err_ren);

    if(rename(tmpfs.virt_add.c_str(), 
                (conf_qmail+"/control/virtualdomains").c_str()))
            lreturn(err_ren);
    skillall("qmail-send", SIGHUP);
    return err_no;
}

/**
*/
char cfsvq::user_pass(const string &u,const string &d,const string &p) throw()
{
    assert_auth();
    string user(lower(u)), dom(lower(d));
    lreturn(! auth->user_pass(user,dom,p) ? err_temp : 0);
}
/**
do user authorization, retrieve user's info
@param ai.user should be set to user's name
@param ai.dom should be set to user's domain
@return 0 true if information was retrieved successful
*/
char cfsvq::user_auth(auth_info & ai) throw()
{
    assert_auth();
    if( ai.user.empty() || ai.dom.empty() ) lreturn(err_user_nf);
    ai.user=lower(ai.user);
    ai.dom=lower(ai.dom);
    switch( auth->user_auth(ai) ) {
    case cauth::user_ok: 
            ai.dir = conf_vq+"/domains/"+split_dom(ai.dom)+'/'+ai.dom+'/'
                    +split_user(ai.user)+'/'+ai.user;
            return 0;
    case cauth::user_nf: lreturn(err_user_nf);
    }
    lreturn(err_temp);
}

/**
*/
char cfsvq::dom_ip_add(const string &d, const string &i) throw()
{
    assert_auth();
    string dom(lower(d)), ip(lower(i));
    lreturn(auth->dom_ip_add(dom,ip) ? 0 : err_temp);
}

/**
*/
char cfsvq::dom_ip_rm(const string &d, const string &i) throw()
{
    assert_auth();
    string dom(lower(d)), ip(lower(i));
    lreturn(auth->dom_ip_rm(dom,ip) ? 0 : err_temp);
}

/**
*/
char cfsvq::dom_ip_rm_all(const string &d) throw()
{
    assert_auth();
    string dom(lower(d));
    lreturn(auth->dom_ip_rm_all(dom) ? 0 : err_temp);
}

/**
*/
char cfsvq::dom_ip_ls(const string &d, vector<string> &ips) throw()
{
    assert_auth();
    string dom(lower(d));
    lreturn(auth->dom_ip_ls(dom,ips) ? 0 : err_temp);
}

/**
*/
char cfsvq::dom_ip_ls_dom(vector<string> &doms) throw()
{
    assert_auth();
    lreturn(auth->dom_ip_ls_dom(doms) ? 0 : err_temp);
}
/**
 * \return path to quota file of specified user@domain
 */
string cfsvq::qtfile(const string &d, const string &u) throw() {
    string dom(lower(d)), user(lower(u));
    string qtfile = conf_vq+"/domains/"+split_dom(dom)+'/'+dom
            +'/'+split_user(user)+'/'+user+"/Maildir/.quota";
    return qtfile;
}
/**
 * \return path to quota file of specified user@domain
 */
string cfsvq::qtfile(const string &d) throw() {
    string dom(lower(d));
    string qtfile = conf_vq+"/domains/"+split_dom(dom)+'/'+dom
            +"/.quota";
    return qtfile;
}
/**
 * \return path to default quota file of specified domain
 */
string cfsvq::qtfile_def(const string &d) throw() {
    string dom(lower(d));
    string qtfile = conf_vq+"/domains/"+split_dom(dom)+'/'+dom
            +"/.quota-default";
    return qtfile;
}
/**
 * \return path to quota file of specified domain
 */
string cfsvq::qtfile_def() throw() {
    return conf_vq+"/domains/.quota-default";
}
/**
 * Set current usage of user's mailbox.
 * \param d domain's name
 * \param u user's name
 * \param c value added to current quota (can be <0)
 */
uint8_t cfsvq::qtf_cur_set(const string &qtfile, int32_t c) throw() {
    quota_value cq;
    fstream qtf(qtfile.c_str(), ios::in | ios::out | ios::nocreate);
    if(!qtf) lreturn(err_qt_no);
    qtf.read(&cq, sizeof(cq));
    if ( c < 0 ) {
      if( -c > cq ) cq = 0;
      else cq+=c;
    } else if( c > 0 ) {
      if ( cq >= 0xffffffff/2 && cq<=c ) cq = 0xffffffff;
      else cq += c;
    } else lreturn(err_no);
    qtf.seekp(0);
    qtf.write(&cq,sizeof(cq));
    qtf.flush();
    qtf.close();
    if(!qtf) lreturn(err_qt_sf);
    lreturn(err_no);
}

uint8_t cfsvq::qt_cur_set(const string&d, const string &u, int32_t c) throw() {
    lreturn(qtf_cur_set(qtfile(d,u), c));
}

/**
*/
uint8_t cfsvq::qt_dom_cur_set(const string &d, int32_t c) throw() {
    lreturn(qtf_cur_set(qtfile(d), c));
}

/**
 * \param d domain's name
 * \param u user's name
 * \param c max quota
*/
uint8_t cfsvq::qtf_set(const string &qtfile, quota_value c) throw() {
    fstream qtf(qtfile.c_str(), ios::in | ios::out | ios::nocreate);
    quota_value z = 0;
    if(qtf) {
            qtf.read(&z,sizeof(z));
            qtf.clear();
            qtf.seekp(0, ios::beg);
    } else {
            qtf.open(qtfile.c_str(), ios::out, conf_vq_fmode_i );
            if(!qtf) lreturn(err_qt_no);
            if(fchown(qtf.rdbuf()->fd(), conf_vq_uid_i, conf_vq_gid_i ) )
                    lreturn(err_qt_sf);
    }            
    qtf.write(&z,sizeof(z));
    qtf.write(&c,sizeof(c));
    qtf.flush();
    qtf.close();
    if(!qtf) lreturn(err_qt_sf);
    lreturn(err_no);
}

uint8_t cfsvq::qt_set(const string &d, const string &u, quota_value c) throw() {
    lreturn(qtf_set(qtfile(d,u), c));
}

uint8_t cfsvq::qt_dom_set(const string &d, quota_value c) throw() {
    lreturn(qtf_set(qtfile(d), c));
}

/**
 * \param d domain's name
 * \param u user's name
 * \param cm quota max
 * \param cc value of current use
*/
uint8_t cfsvq::qtf_set(const string &qtfile, quota_value cm, quota_value cc) throw() {
    ofstream qtf(qtfile.c_str(), ios::nocreate);
    if(!qtf) lreturn(err_qt_no);
    qtf.write(&cc,sizeof(cc));
    qtf.write(&cm,sizeof(cm));
    qtf.flush();
    qtf.close();
    if(!qtf) lreturn(err_qt_sf);
    lreturn(err_no);
}

uint8_t cfsvq::qt_set(const string &d, const string &u, quota_value qm, quota_value qc) throw() {
    lreturn(qtf_set(qtfile(d,u), qm, qc));
}

uint8_t cfsvq::qt_dom_set(const string &d, quota_value qm, quota_value qc) throw() {
    lreturn(qtf_set(qtfile(d), qm, qc));
}

/**
 * \param d domain's name
 * \param u user's name
 * \param cm changed to quota max
 * \param cc changed to current use
*/
uint8_t cfsvq::qtf_get(const string &qtfile, quota_value & cm, quota_value & cc) throw() {
    ifstream qtf(qtfile.c_str(), ios::nocreate);
    if(!qtf) lreturn(err_qt_no);
    qtf.read(&cc,sizeof(cc));
    qtf.read(&cm,sizeof(cm));
    qtf.close();
    if(!qtf) lreturn(err_qt_gf);
    lreturn(err_no);
}

uint8_t cfsvq::qt_get(const string &d, const string &u, 
        quota_value &qm, quota_value & qc) throw() {
    lreturn (qtf_get(qtfile(d,u).c_str(), qm, qc));
}

uint8_t cfsvq::qt_dom_get(const string&d, quota_value & qm, quota_value & qc) throw() {
    lreturn (qtf_get(qtfile(d).c_str(), qm, qc));
}

/**
 * \brief check wheter user used his quota
 * \return 0 - means no, ... - yes, user over quota
*/
uint8_t cfsvq::qtf_over(const string &qtfile) throw() {
    ifstream qtf(qtfile.c_str(), ios::nocreate);
    if(!qtf) return 0; // no quota file
    quota_value qc, qm;
    qtf.read(&qc,sizeof(qc));
    qtf.read(&qm,sizeof(qm));
    if(!qtf || !qm) return 0;
    return qc < qm ? 0 : 1;
}

uint8_t cfsvq::qt_over(const string &d, const string &u) throw() {
    return qtf_over(qtfile(d,u));
}

uint8_t cfsvq::qt_dom_over(const string &d) throw() {
    return qtf_over(qtfile(d));
}

/**
 * set default user quota for this domain
 */
uint8_t cfsvq::qt_def_set(const string &d, quota_value qm) throw() {
    lreturn(qtf_set(qtfile_def(d), qm));
}
/**
 * set default quota for domains
 */
uint8_t cfsvq::qt_dom_def_set(quota_value qm) throw() {
    lreturn(qtf_set(qtfile_def(), qm));
}

/**
 * get default user quota for this domain
 */
uint8_t cfsvq::qt_def_get(const string &d, quota_value & qm) throw() {
    quota_value qc;
    lreturn(qtf_get(qtfile_def(d), qm, qc));
}

/**
 * get default quota for domains
 */
uint8_t cfsvq::qt_dom_def_get(quota_value & qm) throw() {
    quota_value qc;
    lreturn(qtf_get(qtfile_def(), qm, qc));
}

/**
 * set user's quota from default domain's quota
 */
uint8_t cfsvq::qt_def_cp(const string &d, const string &u) throw() {
    uint8_t ret;
    quota_value qc, qm;
    ret = qtf_get(qtfile_def(d), qm, qc);
    if( ret ) lreturn(ret);
    lreturn(qt_set(d,u,qm));
}
/**
 * set domain's quota from default quota
 */
uint8_t cfsvq::qt_dom_def_cp(const string &d) throw() {
    uint8_t ret;
    quota_value qc, qm;
    ret = qtf_get(qtfile_def(), qm, qc);
    if( ret ) lreturn(ret);
    lreturn(qt_dom_set(d,qm));
}

/**
 *
 */
void cfsvq::assert_auth() throw (logic_error) {
    if(!auth) throw logic_error("cfsvq - I want to operate on auth but it's NULL!");
}
/**
 *
 */
uint8_t cfsvq::udot_add(const string &d, const string &u, const string &e,
        udot_info & ui ) throw() {
    assert_auth();
    string dom(lower(d)), user(lower(u)), ext(lower(e));
    string df(dotfile(dom, user, ext));
    string ln(udot_ln(ui));
    if(ln.empty()) lreturn(err_no);

    // is it portable?? may be on some systems it will block?
    ofstream dout;
    string dftmp;
    if( tmp_crt(df, dout, dftmp) != err_no ) lreturn(lastret);
    
    ifstream din(df.c_str());
    if(din) {
            string cln;
            while(getline(din, cln)) {
                    if(cln==ln) lreturn(err_udot_ex);
                    if(!cln.empty()) {
                            dout<<cln<<endl;
                            if(!dout) lreturn(err_wr);
                    }
            }
            if(din.bad()) lreturn(err_rd);
    }

    dout<<ln<<endl;
    dout.flush();
    if(!dout) lreturn(err_wr);

    if(! auth->udot_add(dom,user,ext,ui)) lreturn(err_auth);
    lreturn(rename(dftmp.c_str(), df.c_str()) ? err_ren : err_no);
}
/**
 *
 */
uint8_t cfsvq::udot_rm(const string &d, 
                const string &user, const string &ext,
                const string & id) throw(logic_error) {
    assert_auth();
    udot_info ui;
    ui.id = id;
    if(udot_get(d, ui)) lreturn(lastret);

    string dom(lower(d));
    string df(dotfile(dom, lower(user), lower(ext)));
    string ln(udot_ln(ui));
    if(ln.empty()) lreturn(err_no);

    // is it portable?? may be on some systems it will block?
    ofstream dout;
    string dftmp;
    if( tmp_crt(df, dout, dftmp) != err_no ) lreturn(lastret);
    
    ifstream din(df.c_str());
    if(din) {
            string cln;
            while(getline(din, cln)) {
                    if(cln!=ln && !cln.empty()) {
                            dout<<cln<<endl;
                            if(!dout) lreturn(err_wr);
                    }
            }
            if(din.bad()) lreturn(err_rd);
            din.clear();
    }

    dout.flush();
    if(!dout) lreturn(err_wr);

    if(! auth->udot_rm(dom,id)) lreturn(err_auth);
    lreturn(rename(dftmp.c_str(), df.c_str()) ? err_ren : err_no);
}
/**
 *
 */
uint8_t cfsvq::udot_ls(const string &d, const string &u,
        const string &e, vector<udot_info> & uideq) throw() {
    assert_auth();
    string dom(lower(d)), user(lower(u));
    if(! auth->udot_ls(dom,user,lower(e),uideq)) lreturn(err_auth);
    lreturn(err_no);
}
/**
 *
 */
uint8_t cfsvq::udot_ls(const string &d, const string &u,
        const string &e, char type, vector<udot_info> & uideq) throw() {
    assert_auth();
    string dom(lower(d)), user(lower(u));
    if(! auth->udot_ls(dom,user,lower(e),type,uideq)) lreturn(err_auth);
    lreturn(err_no);
}
/**
 *
 */
uint8_t cfsvq::udot_rep(const string &d, const string &u, 
        const string &e, const udot_info & uin) throw() {
    assert_auth();

    udot_info uio;
    uio.id = uin.id;
    if(udot_get(d, uio)) lreturn(lastret);

    string dom(lower(d));
    string df(dotfile(dom, lower(u), lower(e)));
    string lno(udot_ln(uio)), lnn(udot_ln(uin));
    if(lno.empty()) lreturn(err_no);

    // is it portable?? may be on some systems it will block?
    ofstream dout;
    string dftmp;
    if( tmp_crt(df, dout, dftmp) != err_no ) lreturn(lastret); 
    
    ifstream din(df.c_str());
    if(din) {
            string cln;
            while(getline(din, cln)) {
                    if(cln!=lno && !cln.empty()) {
                            dout<<cln<<endl;
                            if(!dout) lreturn(err_wr);
                    }
            }
            if(din.bad()) lreturn(err_rd);
            din.clear();
    }
    dout<<lnn<<endl;
    dout.flush();
    if(!dout) lreturn(err_wr);

    if(! auth->udot_rep(dom,uin)) lreturn(err_auth);
    lreturn(rename(dftmp.c_str(), df.c_str()) ? err_ren : err_no);
}
/**
 *
 */
uint8_t cfsvq::udot_get(const string &dom, udot_info &ui) throw (logic_error) {
    assert_auth();
    lreturn(! auth->udot_get(lower(dom), ui) ? err_auth : err_no);
}
/**
 *
 */
string cfsvq::dotfile(const string &d, const string &u, const string &e)
        throw() {
    string dom(d), user(u), ext(e);
    string dotfile;
    dotfile.reserve(2000);
    dotfile.append(conf_vq);
    dotfile.append("/domains/");
    dotfile.append(split_dom(dom));
    dotfile.append("/", 1);
    dotfile.append(dom);
    dotfile.append("/", 1);
    dotfile.append(split_user(user));
    dotfile.append("/.qmail-");
    replace(user.begin(),user.end(), '.', ':');
    dotfile.append(user);
    if(!ext.empty()) {
            replace(ext.begin(),ext.end(), '.', ':');
            dotfile += '-'+ext;
    }
    return dotfile;
}
/**
 *
 */
string cfsvq::udot_sup() throw() {
    return string(ud_sup, sizeof(ud_sup));
}
/**
 * \return 0 if type is not supported
 */
uint8_t cfsvq::udot_sup_is(char t) throw() {
    unsigned ud_supl = sizeof(ud_supl);
    if(!ud_supl) return 0;
    ud_supl--;
    do {
            if(ud_sup[ud_supl] == t) return 1;
    } while(ud_supl--);
    return 0;
}
/**
 * \brief lock deliveries to specified domain
 * \return 0 on success
 */
uint8_t cfsvq::dom_del_lock( const string & dom ) throw() {
    struct stat st;
    if(stat(dom.c_str(), &st)) lreturn(err_st);
    if(! S_ISDIR(st.st_mode)) lreturn(err_notdir);
    lreturn( chmod(dom.c_str(), (st.st_mode & 07777) | 01000) ? err_chmod : 0 );
}

/**
 * \brief unlock delivieries to specified domain
 * \return 0 on success
 */
uint8_t cfsvq::dom_del_unlock( const string & dom ) throw() {
    struct stat st;
    if(stat(dom.c_str(), &st)) lreturn(err_st);
    if(! S_ISDIR(st.st_mode)) lreturn(err_notdir);
    lreturn( chmod(dom.c_str(), (st.st_mode & 07777) & ~01000) ? err_chmod : 0 );
}

/**
 *
 */
string cfsvq::dompath(const string &dom) throw() {
    string ret;
    ret.reserve(conf_vq.length()+10+2*dom.length());
    ret = conf_vq;
    ret += "/domains/";
    ret += split_dom(dom);
    ret += dom;
    return ret;
}

/**
 *
 */
string cfsvq::udot_ln(const udot_info &ui) throw() {
    string ret;
    switch(ui.type) {
    case redir:
            ret.reserve(ui.val.length()+3);
            ret = '&';
            ret += ui.val;
            break;
    default:
            ret = ui.val;
    }
    return ret;
}

/**
 *
 */
uint8_t cfsvq::udot_has(const string &dom, const string &user, 
        const string &ext, char type) {
    assert_auth();

    return auth->udot_has(dom, user, ext, type);
}

/**
 *
 */
uint8_t cfsvq::udot_add_md_def(const string &d, const string &u,
        const string &e) {
    udot_info ui;
    ui.type = maildir;
    ui.val = "./"+lower(u)+"/Maildir/";
    lreturn(udot_add(d, u, e, ui));
}

/**
 * creates temporary file, chown(vq), chmod(vq_mode)
 */
uint8_t cfsvq::tmp_crt(const string &fn, ofstream & out, string &fnout) throw() {
    ostringstream pid;
    pid<<getpid();
    
    fnout = fn + '.' + pid.str() + tmp_end;
    if(out.is_open()) out.close();
    out.open(fnout.c_str(), ios::trunc, conf_vq_fmode_i );
    if(!out) lreturn(err_open);
    if( fchown(out.rdbuf()->fd(), conf_vq_uid_i, conf_vq_gid_i) )
            lreturn(err_chown);
    if( fchmod(out.rdbuf()->fd(), conf_vq_fmode_i ) )
            lreturn(err_chmod);
    lreturn(err_no);
}
/**
 *
 */
uint8_t cfsvq::udot_rm(const string &d, const string &u,
        const string &e, char type) {
    assert_auth();
    vector<udot_info> uis;
    vector<udot_info>::iterator uis_end, uis_cur;
    if(udot_ls(d, u, e, type, uis)) lreturn(lastret);
    uis_end = uis.end();
    
    string dom(lower(d));
    string df(dotfile(dom, lower(u), lower(e)));

    for( uis_cur=uis.begin(); uis_cur != uis_end; ++ uis_cur ) {
            uis_cur->val = udot_ln(*uis_cur);
    }

    // is it portable?? may be on some systems it will block?
    ofstream dout;
    string dftmp;
    if( tmp_crt(df, dout, dftmp) != err_no ) lreturn(lastret);
    
    ifstream din(df.c_str());
    if(din) {
            string cln;
            while(getline(din, cln)) {
                    if( cln.empty() ) continue;
                    for( uis_cur = uis.begin(); uis_cur != uis_end; uis_cur ++ ) {
                            if( cln == uis_cur->val ) break;
                    }
                    if( uis_cur == uis_end ) {
                            dout<<cln<<endl;
                            if(!dout) lreturn(err_wr);
                    }
            }
            if(din.bad()) lreturn(err_rd);
            din.clear();
    }

    dout.flush();
    if(!dout) lreturn(err_wr);
    if(! auth->udot_rm(dom,u,e,type)) lreturn(err_auth);
    lreturn(rename(dftmp.c_str(), df.c_str()) ? err_ren : err_no);
}

/**
 *
 */
uint8_t cfsvq::udot_type_cnt( const string &d, const string &u, const string &e,
        char type, size_type &cnt) throw() {
    assert_auth();
    lreturn( ! auth->udot_type_cnt(d,u,e,type,cnt) ? err_auth : err_no );
}
/**
 *
 */
uint8_t cfsvq::user_ex( const string &dom, const string &user ) throw() {
    assert_auth();
    return auth->user_ex(dom, user);
}
