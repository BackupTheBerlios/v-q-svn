#ifndef __VQWWW_CONF_H
#define __VQWWW_CONF_H 

#include "cconf.h"

namespace vqwww {
		
extern clnconf ac_fs; 
extern cintconf ac_fs_split; 
extern clnconf ac_html_br; 
extern cintconf ac_pass_maxl; 
extern cintconf ac_pass_minl; 
extern clnconf ac_pass_ok; 
extern cintconf ac_redir_max; 
extern cintconf ac_sess_timeout; 
extern cintconf ac_sess_fmode;
extern cintconf ac_user_maxl; 
extern cintconf ac_user_minl; 
extern clnconf ac_user_ok; 
extern cgidconf ac_vq_gid; 
extern cuidconf ac_vq_uid; 
extern clnconf ac_xmlstart; 
extern clnconf ac_autoresp_cp;
extern cmapconf ac_codepages;
extern cmapconf ac_locales;

} // namespace vqwww
#endif
