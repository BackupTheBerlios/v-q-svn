#ifndef __CINFODB_H
#define __CINFODB_H

#include <inttypes.h>
#include <string>
#include <vector>

using namespace std;

class cinfodb {
public:
    enum {
            err_no = 0, //!< no error
            err_read, //!< can't read from database
            err_write, //!< can't write to database
            err_noent, //!< entry not found
    } err;

    typedef struct {
        std::string id;
        std::string dir;
        std::string prefix;
    } wwwali_info;

    typedef struct {
        std::string id;
        std::string domain;
    } wwwali_dom_info;
    
    virtual uint8_t wwwali_ls(const std::string &, std::vector<wwwali_dom_info> &)=0;
    virtual uint8_t wwwali_dom_ls(const std::string &, std::vector<wwwali_info> & )=0;
    virtual uint8_t wwwali_dom_ls_dir(const std::string &, 
                    const std::string &, std::vector<wwwali_info> & )=0;
    virtual uint8_t wwwali_dom_id_get(const std::string &, std::string &)=0;
    virtual uint8_t wwwali_rep(const std::string &, const wwwali_info & )=0;
    virtual uint8_t wwwali_rm(const std::string &, const std::string &)=0;
    virtual uint8_t wwwali_add(const std::string &, wwwali_info &)=0;
    virtual uint8_t wwwali_get(const std::string &, wwwali_info &)=0;
    virtual uint8_t wwwali_dom_get(const std::string &, std::string &)=0;
    virtual uint8_t wwwali_rdom_get(const std::string &, std::string &)=0;

    typedef struct {
        std::string id; //!< entry's id
        std::string user; //!< user
        std::string date_crt; //!< account's creation date
        std::string date_exp; //!< expire date
    } user_info;
    
    virtual uint8_t user_ls(const std::string &, std::vector<user_info> & )=0;
    
    typedef struct {
        std::string id; //!< entry's id
        std::string user; //!< user
        std::string dir; //!< subdirectory
    } ftp_info;
    
    /// adds user which can log into some part of ftp server
    virtual uint8_t ftp_add(const std::string &, const ftp_info &)=0;
    virtual uint8_t ftp_rm(const std::string &, const std::string &)=0;
    virtual uint8_t ftp_ls(const std::string &, std::vector<ftp_info> & )=0;
    virtual uint8_t ftp_rep(const std::string &, const ftp_info &)=0;
    virtual uint8_t ftp_get(const std::string &, ftp_info &)=0; //!< get ftp_info for specified user

    virtual uint8_t ui_id_get(const std::string &, const std::string &, std::string &)=0;
};

#endif
