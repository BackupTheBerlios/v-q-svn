#ifndef __ADS_H
#define __ADS_H

#include <string>
#include <deque>
#include <stdexcept>
#include <inttypes.h>

using namespace std;

class cads_runtime : public runtime_error 
{
        protected:
                string m;
        public:
                cads_runtime() {}
                cads_runtime(const string &msg) : m(msg) {}
                const char *what() const { return m.c_str(); }
};

class cads {
        public:
                typedef uint32_t size_type;
                typedef struct { 
                        string gid, aid, misc, url, asis, asisurl; 
                        size_type mc, cc, ms, cs, ttl;
                        uint8_t blk;
                } ad_info;

                /// initialization
                virtual bool init()=0;
                /// add a group, return true on success, false on error
                virtual bool grps_add( const string & )=0;
                /// returns group names and their ids
                virtual bool grps( deque <string> &, deque <string> & )=0;
                /// removes selected groups (by id)
                virtual bool grps_rm( const deque <string> & )=0;
                ///
                virtual bool grps_rm( const string & ) = 0;
                /// changes group's name
                virtual bool grps_chg( const string &, const string & )=0;
                /// add advertisement
                virtual bool ads_add( const string &, const string &,
                                size_type, size_type, size_type, 
                                const string &, const string & ) =0;
                /// remove advertisements
                virtual bool ads_rm( const string &, const deque<string> & )=0;
                /// remove ad.
                virtual bool ads_rm( const string &, const string &) = 0;
                /// get random ad.
                virtual bool ads_get(string &,string &,string &)=0;
                /// get ad. from specified group
                virtual bool ads_get_grp(const string &, string &, string &)=0;
                /// get url of specified ad.
                virtual bool ads_get_grp_ad(const string &, const string &, string &)=0;
                /// get host url
                virtual bool ads_get_url(const string &, const string &, string & )=0;
                /// get all info on all ads. in specified group
                virtual bool ads_ls( const string &, deque <ad_info> & ) =0;
                /// get info about ad.
                virtual bool ads_info( const string &, ad_info &)=0;
                /// change info
                virtual bool ads_chg( const string &, const ad_info &) =0;
                /// (un)block ad.
                virtual bool ads_blk( const string &, const string &, uint8_t )=0;
                /// returns last error message
                virtual const char * err()=0;
};

#endif
