#ifndef __CDISKADS_H
#define __CDISKADS_H

#include <fstream>
#include <sys/types.h>
#include <string>
#include <errno.h>

#include "cads.h"

using namespace std;

class cdiskads_runtime : public cads_runtime {
        public:
                cdiskads_runtime(const string &msg, bool e=false) : cads_runtime(msg)
                        { if(e) { m+= (string)": " + strerror(errno); } }
};

class cdiskads : public cads {
        public:
                typedef uint32_t size_type;
                
        protected:
                size_type ZERO;
                static const uint8_t id_len = sizeof(uint32_t)*2;
                string errmsg;
                string dir, wdir;

                fstream ads_nextid;
                struct {
                        fstream fs;
                        string name;
                        size_type val;
                } next, nextid, cids;
                        
                void crt( fstream &, const string & ) throw(cdiskads_runtime);
                string id2hex( const char * ) throw();
                bool cp(fstream &,const string &, fstream &) throw();
                bool nextid_get( string & ) throw();
                bool ads_get_grp_ad(const string &, const string &,string &,
                                fstream & ) throw();
                bool ads_info_inc(const string &,const string & ) throw();
                
        public:

                cdiskads( const char *, const char * ) throw();
                virtual ~cdiskads() throw() {}
                /// initialization
                bool init() throw(cdiskads_runtime);
                /// add a group, return true on success, false on error
                bool grps_add( const string & ) throw(cdiskads_runtime);
                /// returns group names and their ids
                bool grps( deque <string> &, deque <string> & ) throw();
                /// removes selected groups (by id)
                bool grps_rm( const deque <string> & ) throw(cdiskads_runtime);
                /// 
                bool grps_rm( const string & ) throw(cdiskads_runtime);
                /// changes group's name
                bool grps_chg( const string &, const string & ) throw();
                /// get next id of an ad. in a specified group
                bool ads_nextid_get( const string &, string & ) throw();
                /// add advertisement
                bool ads_add( const string &, const string &,
                                size_type, size_type, size_type, 
                                const string &, const string & ) 
                        throw(cdiskads_runtime);
                /// remove advertisements
                bool ads_rm( const string &, const deque<string> & )
                        throw(cdiskads_runtime);
                /// remove ad.
                bool ads_rm( const string &, const string & )
                        throw(cdiskads_runtime);
                /// get random ad.
                bool ads_get(string &,string &,string &) throw();
                /// get ad. from specified group
                bool ads_get_grp(const string &,string &,string &) throw();
                /// get url of specified ad.
                bool ads_get_grp_ad(const string &,const string &, string &)
                        throw();
                /// get host url
                bool ads_get_url(const string &,const string &, string &) throw();
                /// get all info about specified ads.
                bool ads_ls( const string &, deque<ad_info> & )
                        throw();
                /// (un)block ad.
                bool ads_blk( const string &, const string &, uint8_t )
                        throw();
                /// get info about specified ad.
                bool ads_info( const string &, ad_info & )
                        throw();
                /// change info
                bool ads_chg( const string &, const ad_info & ) throw();
                /// returns last error message
                const char * err() throw();
};

#endif
