/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new@foo-baz.com)

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
#include "cdbdaemon.h"
#include "conf_home.h"
#include "freemail_conf.h"
#include "daemon.h"

#include <sig.h>
#include <fd.h>
#include <fdstr.h>
#include <util.h>
#include <lock.h>
#include <sys.h>

#define HAVE_NAMESPACE_STD 1
#include <pqxx/all.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctime>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;
using namespace freemail;

// socket
int slock;
// pgsql
pqxx::Connection *pg = NULL;
// child
char msg_type=0;
string id, localeinfo, login, dom_id, country, city, birthday, area_id, work_id;
string edu_id, sex_id, re_que, re_ans, domain;
vector<string> ints;
vector<string>::size_type ints_cnt;

/**
@exception runtime_error if it can't create socket
*/
void setup()
{
    umask(0);
    if(!sig_pipe_ign() || !sig_chld_nocldwait() ) 
            throw runtime_error((string)"setup: can't set signals: "+strerror(errno));
    if(chdir((conf_home+"/var/run/vqsignup1d").c_str()))
            throw runtime_error((string)"setup: chdir: "
                    +conf_home+"/var/run: "+strerror(errno));
    slock = open("vqsignup1d.lock", O_RDONLY | O_NONBLOCK | O_CREAT, 0666 );
    if(slock==-1)
            throw runtime_error((string)"setup: open: vqsignup1d.lock: "+strerror(errno));
    if(lock_exnb(slock)) {
            if( errno == EWOULDBLOCK )
                    throw runtime_error((string)"setup: vqsignup1d.lock is already locked");
            else throw runtime_error((string)"setup: vqsignup1d.lock: lock: "+strerror(errno));
    }
    if(unlink("vqsignup1d.sock")==-1 && errno != ENOENT )
            throw runtime_error((string)"setup: unlink: vqsignup1d.sock: "+strerror(errno));
    if( (sso=socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
            throw runtime_error((string)"setup: socket: "+strerror(errno));
    struct sockaddr_un sun;
    sun.sun_family = AF_UNIX;
    strcpy(sun.sun_path, "vqsignup1d.sock");
    if(bind(sso, (struct sockaddr*) &sun, SUN_LEN(&sun)))
            throw runtime_error((string)"setup: bind: vqsignup1d.sock: "+strerror(errno));
    if( chmod(sun.sun_path, 0600) )
            throw runtime_error((string)"setup: chmod: vasignup1d.sock: "+strerror(errno));
    if( !(pg = new pqxx::Connection(ac_pgsql.val_str().c_str())) ) 
            throw runtime_error((string)"setup: no memory"); 
    if( listen(sso, 5) == -1 )
            throw runtime_error((string)"setup: listen: "+strerror(errno));
}

/**
 * Sends 2 strings from result.
 */
void send2strings( const pqxx::Result::Tuple & tup ) {
	const char *ptr;
	string::size_type ptrl;
	ptr = tup[0].c_str();
	if(!ptr) ptr = "";
	ptrl = strlen(ptr);
	if( fdwrite(cso, &ptrl, sizeof(ptrl)) == -1
		|| fdwrite(cso, ptr, ptrl) == -1 ) 
			throw wr_error();
				
	ptr = tup[1].c_str();
	if(!ptr) ptr = "";
	ptrl = strlen(ptr);
	if( fdwrite(cso, &ptrl, sizeof(ptrl)) == -1
		|| fdwrite(cso, ptr, ptrl) == -1 ) 
			throw wr_error();
}

/**
 * get education levels
 */
void cmd_edu_ls() {
	if( fdrdstr(cso, localeinfo) != -1 ) {
			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"SELECT id,name FROM users_info_edu WHERE locale="
		         +pqxx::Quote(localeinfo, false)+" ORDER BY name"));

			vector<cdbdaemon::edu_info>::size_type i, cnt = res.size();
		    if( cnt ) {
			    	if( fdwrite(cso, &"F", 1) != 1 
						|| fdwrite(cso, &cnt, sizeof(cnt)) != sizeof(cnt) )
							throw wr_error();
		
					for( i=0; i < cnt; ++i ) {
							send2strings(res[i]);
					}
    			    return;
			} else {
					if( fdwrite(cso, &"Z", 1) != 1 ) throw wr_error();
					return;
			}
 
			if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
	} else 
		 throw rd_error();
}

/**
 * get works
 */
void cmd_work_ls() {
	if( fdrdstr(cso, localeinfo) != -1 ) {
			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"SELECT id,name FROM users_info_work WHERE locale="
				+pqxx::Quote(localeinfo, false)+" ORDER BY name"));

			vector<cdbdaemon::edu_info>::size_type i, cnt = res.size();
			if( cnt ) {
					if( fdwrite(cso, &"F", 1) != 1 
						|| fdwrite(cso, &cnt, sizeof(cnt)) != sizeof(cnt) )
							throw wr_error();
					
        			for( i=0; i < cnt; ++i ) {
							send2strings(res[i]);
					}
					return;
			} else {
					if( fdwrite(cso, &"Z", 1) != 1 ) throw wr_error();
					return;
			}
 
			if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
	} else 
			throw rd_error();
}

/**
 * get interests
 */
void cmd_int_ls() {
	if( fdrdstr(cso, localeinfo) != -1 ) {
			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"SELECT id,name FROM users_info_interests WHERE locale="
				+pqxx::Quote(localeinfo, false)+" ORDER BY name"));

			vector<cdbdaemon::edu_info>::size_type i, cnt = res.size();
			if( cnt ) {
					if( fdwrite(cso, &"F", 1) != 1 
						|| fdwrite(cso, &cnt, sizeof(cnt)) != sizeof(cnt) )
							throw wr_error();

        			for( i=0; i < cnt; ++i ) {
							send2strings(res[i]);
					}
         			return;
			} else {
					if( fdwrite(cso, &"Z", 1) != 1 ) throw wr_error();
					return;
			}
 
   			if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
	} else 
			throw rd_error();
}

/**
 * get interests
 */
void cmd_area_ls() {
	if( fdrdstr(cso, localeinfo) != -1 ) {
			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"SELECT id,name FROM users_info_areas WHERE locale="
				+pqxx::Quote(localeinfo, false)+" ORDER BY name") );
			
			vector<cdbdaemon::edu_info>::size_type i, cnt = res.size();
			if( cnt ) {
					if( fdwrite(cso, &"F", 1) != 1 
						|| fdwrite(cso, &cnt, sizeof(cnt)) != sizeof(cnt) )
							throw wr_error();

         			for( i=0; i < cnt; ++i ) {
							send2strings(res[i]);
					}
         			return;
			} else {
					if( fdwrite(cso, &"Z", 1) != 1 ) throw wr_error();
					return;
     		}
 
			if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
	} else 
			throw rd_error();
}

/**
 * get interests
 */
void cmd_sex_ls() {
		if( fdrdstr(cso, localeinfo) != -1 ) {
				pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
					"SELECT id,name FROM users_info_sexes WHERE locale="
					+pqxx::Quote(localeinfo, false)+" ORDER BY name") );

				vector<cdbdaemon::sex_info>::size_type i, cnt = res.size();
				
				if( cnt ) {
						if( fdwrite(cso, &"F", 1) != 1 
							|| fdwrite(cso, &cnt, sizeof(cnt)) != sizeof(cnt) )
								throw wr_error();

        		 		for( i=0; i < cnt; ++i ) {
								send2strings(res[i]);
						}
        		 		return;
				} else {
						if( fdwrite(cso, &"Z", 1) != 1 ) throw wr_error();
						return;
				}
   				
				if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
	} else 
			throw rd_error();
}

/**
 * get interests
 */
void cmd_dom_ls() {
	if( fdrdstr(cso, localeinfo) != -1 ) {
			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"SELECT id,name FROM domains ORDER BY name") );
			
			vector<cdbdaemon::domain_info>::size_type i, cnt = res.size();
			
			if( cnt ) {
					if( fdwrite(cso, &"F", 1) != 1 
						|| fdwrite(cso, &cnt, sizeof(cnt)) != sizeof(cnt) )
							throw wr_error();

         			for( i=0; i < cnt; ++i ) {
							send2strings(res[i]);
					}
         			return;
     		} else {
					if( fdwrite(cso, &"Z", 1) != 1 ) throw wr_error();
					return;
			}
 
		   if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
	} else throw rd_error();
}

/**
 *
 */
void cmd_ui_rm() {
	if( fdrdstr(cso, id) != -1 ) {
			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"DELETE FROM users_info WHERE id="
				+pqxx::Quote(id, false)) );
			
			if( fdwrite(cso, &"K", 1) != 1 ) throw wr_error();
	} else throw rd_error();
}

/**
 *
 */
void cmd_dom_get() {
	if( fdrdstr(cso, id) != -1 ) {
			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"SELECT name FROM domains WHERE id="
				+pqxx::Quote(id, false)+" LIMIT 1") );
			
			if( ! res.size() ) {
					if( fdwrite(cso, &"Z", 1) != 1 ) throw wr_error();
					return;
			}
			
			const char *did = res[0][0].c_str();
			if(!did) did = "";
			if( fdwrite(cso, &"F", 1) != 1 
				|| fdwrstr(cso, did) == -1 ) 
					throw wr_error();
			return;
     } else throw rd_error();
}

/**
 *
 */
void cmd_dom_get_by_name() {
	if( fdrdstr(cso, id) != -1 ) {
			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"SELECT id FROM domains WHERE name="
				+pqxx::Quote(id, false)+" LIMIT 1") );

			if( ! res.size() ) {
					if( fdwrite(cso, &"Z", 1) != 1 ) throw wr_error();
					return;
			}
			
			const char *did = res[0][0].c_str();
			if(!did) did = "";
			if( fdwrite(cso, &"F", 1) != 1
				|| fdwrstr(cso, did) == -1 )
					throw wr_error();
			return;
	} else throw rd_error();
}

/**
 *
 */
void cmd_ui_add() {
	if( fdrdstr(cso, localeinfo) != -1 
		&& fdrdstr(cso, login) != -1
		&& fdrdstr(cso, dom_id) != -1
		&& fdrdstr(cso, country) != -1
		&& fdrdstr(cso, city) != -1
		&& fdrdstr(cso, birthday) != -1
		&& fdrdstr(cso, area_id) != -1
		&& fdrdstr(cso, work_id) != -1
		&& fdrdstr(cso, edu_id) != -1
		&& fdrdstr(cso, sex_id) != -1
		&& fdrdstr(cso, re_que) != -1 
		&& fdrdstr(cso, re_ans) != -1 
		&& fdread(cso, &ints_cnt, sizeof(ints_cnt)) == sizeof(ints_cnt) ) {
			ints.clear();
			if( ints_cnt -- ) {
					string inter;
					do {
							if( fdrdstr(cso, inter) == -1 )	throw rd_error();
							ints.push_back(inter);
					} while( ints_cnt -- );
			}

			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"SELECT NEXTVAL('users_info_id_seq')") );
			
			if( res.size() == 1 && ! res[0][0].is_null() ) {
					pqxx::Transaction tran(*pg);
					string uid(res[0][0].c_str());
					
					// first query call some procedure returning data, that's why we compare it to PGRES_TUPLES_OK
					if( tran.Exec("INSERT INTO users_info "
						"(id,login,domain_id,country,city,"
						"birthday,re_question,re_answer,locale) VALUES("
							+pqxx::Quote(uid, false)+","
							+pqxx::Quote(login, false)+","
							+pqxx::Quote(dom_id, false)+","
							+pqxx::Quote(country, false)+","
							+pqxx::Quote(city, false)+","
							+pqxx::Quote(birthday, false)+","
							+pqxx::Quote(re_que, false)+","
							+pqxx::Quote(re_ans, false)+","
							+pqxx::Quote(localeinfo, false)+")").AffectedRows() != 1
						|| tran.Exec("INSERT INTO users_info_map_area "
							"(user_id,val_id) VALUES("
							+pqxx::Quote(uid, false)+","
							+pqxx::Quote(area_id, false)+")").AffectedRows() != 1
						|| tran.Exec("INSERT INTO users_info_map_work "
							"(user_id,val_id) VALUES("
							+pqxx::Quote(uid, false)+","
							+pqxx::Quote(work_id, false)+")").AffectedRows() != 1
						|| tran.Exec("INSERT INTO users_info_map_edu "
							"(user_id,val_id) VALUES("
							+pqxx::Quote(uid, false)+","
							+pqxx::Quote(edu_id, false)+")").AffectedRows() != 1
						|| tran.Exec("INSERT INTO users_info_map_sex "
							"(user_id,val_id) VALUES("
							+pqxx::Quote(uid, false)+","
							+pqxx::Quote(sex_id, false)+")").AffectedRows() != 1 ) {

							if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
							return;
					}

					ints_cnt = ints.size();
					
					if( ints_cnt-- ) {
							do {
									if( tran.Exec("INSERT INTO users_info_map_ints "
										"(user_id,val_id) VALUES("
										+pqxx::Quote(uid, false)+","
										+pqxx::Quote(ints[ints_cnt], false)
										+")").AffectedRows() != 1 ) {
											if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
											return;
									}
							} while( ints_cnt -- );
					}

					tran.Commit();
					
					if( fdwrite(cso, &"F", 1) != 1
						|| fdwrstr(cso, uid) == -1 ) throw wr_error();
					return;
			}

			if( fdwrite( cso, &"E", 1) != 1 ) throw wr_error();
	} else throw rd_error();
}

/**
 *
 */
void cmd_ui_chg() {
	if( fdrdstr(cso, localeinfo) != -1 
		&& fdrdstr(cso, id) != -1
		&& fdrdstr(cso, country) != -1
		&& fdrdstr(cso, city) != -1
		&& fdrdstr(cso, birthday) != -1
		&& fdrdstr(cso, area_id) != -1
		&& fdrdstr(cso, work_id) != -1
		&& fdrdstr(cso, edu_id) != -1
		&& fdrdstr(cso, sex_id) != -1
		&& fdrdstr(cso, re_que) != -1 
		&& fdrdstr(cso, re_ans) != -1 
		&& fdread(cso, &ints_cnt, sizeof(ints_cnt)) == sizeof(ints_cnt) ) {
			ints.clear();

			if( ints_cnt -- ) {
					string inter;
					do {
							if( fdrdstr(cso, inter) == -1 ) throw rd_error();
							ints.push_back(inter);
					} while( ints_cnt -- );
			}

			pqxx::Transaction tran(*pg);

			if( tran.Exec("UPDATE users_info SET country="
					+pqxx::Quote(country, false)
					+",city="+pqxx::Quote(city, false)
					+",birthday="+pqxx::Quote(birthday, false)
					+",re_question="+pqxx::Quote(re_que, false)
					+",re_answer="+pqxx::Quote(re_ans, false)
					+",locale="+pqxx::Quote(localeinfo, false)
					+" WHERE id="+pqxx::Quote(id, false)).AffectedRows() != 1
				|| tran.Exec("UPDATE users_info_map_area SET val_id="
					+pqxx::Quote(area_id, false)
					+" WHERE user_id="+pqxx::Quote(id, false)).AffectedRows() != 1
				|| tran.Exec("UPDATE users_info_map_work SET val_id="
					+pqxx::Quote(work_id, false)
					+" WHERE user_id="+pqxx::Quote(id, false)).AffectedRows() != 1
				|| tran.Exec("UPDATE users_info_map_edu SET val_id="
					+pqxx::Quote(edu_id, false)
					+" WHERE user_id="+pqxx::Quote(id, false)).AffectedRows() != 1
				|| tran.Exec("UPDATE users_info_map_sex SET val_id="
					+pqxx::Quote(sex_id, false)
					+" WHERE user_id="+pqxx::Quote(id, false)).AffectedRows() != 1 ) {
					if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
					return;
			}

			tran.Exec("DELETE FROM users_info_map_ints WHERE user_id="
				+pqxx::Quote(id, false));
			
			ints_cnt = ints.size();
			if( ints_cnt-- ) {
					do {
							if( tran.Exec("INSERT INTO users_info_map_ints"
								" (user_id,val_id) VALUES("
								+pqxx::Quote(id, false)+","
								+pqxx::Quote(ints[ints_cnt], false)
								+")").AffectedRows() != 1 ) {
									if(fdwrite(cso, &"E", 1) != 1) throw wr_error();
									return;
							}
					} while( ints_cnt -- );
			}

			tran.Commit();

			if( fdwrite(cso, &"K", 1) != 1 ) throw wr_error();
	} else throw rd_error();
}

/**
 * 
 */
template <class T>
	uint8_t ui_elem_get( const pqxx::Result & res, const string & loc,
		T & info ) {

	int tups;
	if( ! (tups=res.size()) ) return cdbdaemon::err_empty;
	if( tups > 2 ) return cdbdaemon::err_read;

	int fidx = 0;
	string locale;
	const char *tmp;

	tmp = res[0][fidx++].c_str();
	if(!tmp) tmp = "";
	info.id = tmp;
		
	tmp = res[0][fidx++].c_str();
	if(!tmp) tmp = "";
	info.name = tmp;

	tmp = res[0][fidx++].c_str();
	if(!tmp) tmp = "";
	locale = tmp;
		
	if( tups == 1 || loc == locale ) {
			return cdbdaemon::err_no;
	}
		
	fidx = 0;
	tmp = res[1][fidx++].c_str();
	if(!tmp) tmp = "";
	info.id = tmp;

	tmp = res[1][fidx++].c_str();
	if(!tmp) tmp = "";
	info.name = tmp;

	return cdbdaemon::err_no;
}

/**
 *
 */
uint8_t ui_work_get(const string &uid, const string &loc, cdbdaemon::work_info & info ) {
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT id,name,locale FROM users_info_work"
		" WHERE (code=(SELECT code FROM users_info_work WHERE id="
		"(SELECT val_id FROM users_info_map_work WHERE user_id="
		+pqxx::Quote(uid, false)+")) AND locale="+pqxx::Quote(loc, false)
		+") OR code=(SELECT code FROM users_info_map_work WHERE id="
		"(SELECT val_id FROM users_info_map_work WHERE user_id="
		+pqxx::Quote(uid, false)+"))") );

	return ui_elem_get<cdbdaemon::work_info>(res, loc, info);
}

/**
 *
 */
uint8_t ui_area_get(const string &uid, const string &loc, cdbdaemon::area_info & info ) {
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT id,name,locale FROM users_info_areas "
		"WHERE (code=(SELECT code FROM users_info_areas WHERE id="
		"(SELECT val_id FROM users_info_map_area WHERE user_id="
		+pqxx::Quote(uid, false)+")) AND locale="+pqxx::Quote(loc, false)+") "
		"OR code=(SELECT code FROM users_info_map_area WHERE id="
		"(SELECT val_id FROM users_info_map_area WHERE user_id="
		+pqxx::Quote(uid, false)+"))"));
	
	return ui_elem_get<cdbdaemon::area_info>(res, loc, info);
}

/**
 *
 */
uint8_t ui_sex_get(const string &uid, const string &loc, cdbdaemon::sex_info & info ) {
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT id,name,locale FROM users_info_sexes "
		"WHERE (code=(SELECT code FROM users_info_sexes WHERE id="
		"(SELECT val_id FROM users_info_map_sex WHERE user_id="
		+pqxx::Quote(uid, false)+")) AND locale="+pqxx::Quote(loc, false)+") "
		"OR code=(SELECT code FROM users_info_map_sex WHERE id="
		"(SELECT val_id FROM users_info_map_sex WHERE user_id="
		+pqxx::Quote(uid, false)+"))") );
	
	return ui_elem_get<cdbdaemon::sex_info>(res, loc, info);
}

/**
 *
 */
uint8_t ui_edu_get(const string &uid, const string &loc, cdbdaemon::edu_info & info ) {
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT id,name,locale FROM users_info_edu "
		"WHERE (code=(SELECT code FROM users_info_edu WHERE id="
		"(SELECT val_id FROM users_info_map_edu WHERE user_id="
		+pqxx::Quote(uid, false)+")) AND locale="+pqxx::Quote(loc, false)+") "
		"OR code=(SELECT code FROM users_info_map_edu WHERE id="
		"(SELECT val_id FROM users_info_map_edu WHERE user_id="
		+pqxx::Quote(uid, false)+"))") );

   return ui_elem_get<cdbdaemon::edu_info>(res, loc, info);
}

/**
 *
 */
uint8_t ui_ints_get(const string &uid, vector<cdbdaemon::int_info> & infos ) {
	infos.clear();

	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT ui.id,ui.name FROM users_info_map_ints AS umi,"
		"users_info_interests AS ui WHERE umi.user_id="+pqxx::Quote(uid, false)+
		" AND umi.val_id=ui.id") );

	const char *tmp;
	int tups;
	cdbdaemon::int_info inter;

	if( (tups=res.size()) == 0 ) return cdbdaemon::err_empty;
	tups--;
	do {
			tmp = res[tups][0].c_str();
			if(!tmp) tmp = "";
			inter.id = tmp;

			tmp = res[tups][1].c_str();
			if(!tmp) tmp = "";
			inter.name = tmp;

			infos.push_back(inter);
	} while( tups -- );
	
	return cdbdaemon::err_no;
}

/**
 *
 */
void cmd_ui_get_by_name() {
	if( fdrdstr(cso, localeinfo) == -1 
		|| fdrdstr(cso, login) == -1
		|| fdrdstr(cso, dom_id) == -1 )
			throw rd_error();
	
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT ui.id,ui.country,ui.city,ui.birthday,"
		"ui.re_question,ui.re_answer,ui.locale,uw.id,uw.name,us.id,us.name,"
		"ue.id,ue.name,ua.id,ua.name"
		" FROM users_info as ui,users_info_work as uw,users_info_edu as ue,"
		"users_info_areas as ua,users_info_sexes as us,"
		"users_info_map_edu as ume,users_info_map_work as umw,"
		"users_info_map_area as uma,users_info_map_sex as ums where"
		" ui.login="+pqxx::Quote(login, false)+" and ui.domain_id="
		+pqxx::Quote(dom_id, false)+" AND ui.id=umw.user_id and umw.val_id=uw.id and "
		"ui.id=ume.user_id and ume.val_id=ue.id and ui.id=ums.user_id"
		" and ums.val_id=us.id and ui.id=uma.user_id and uma.val_id=ua.id") );

	if( res.size() != 1 ) {
			if( fdwrite(cso, &"Z", 1) != 1 ) throw wr_error();
			return;
	}
			
	const char *tmp;
	int fidx = 0;
	cdbdaemon::ui_full ui;
	vector<cdbdaemon::int_info>::size_type icnt;

	tmp = res[0][fidx++].c_str();
	if(!tmp) tmp = "";
	ui.id = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.country = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.city = tmp;
			
	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.birthday = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.re_que = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.re_ans = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.locale = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.work.id = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.work.name = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.sex.id = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.sex.name = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.edu.id = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.edu.name = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.area.id = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.area.name = tmp;

	switch( ui_ints_get(ui.id, ui.ints) ) {
	case cdbdaemon::err_no:
	case cdbdaemon::err_empty:
			break;
	default:
   			if( fdwrite( cso, &"E", 1) != 1 ) throw wr_error();
			return;
	}

	icnt = ui.ints.size();

	if( fdwrite(cso, &"F", 1) != 1
		|| fdwrstr(cso, ui.id) == -1 
		|| fdwrstr(cso, ui.country) == -1 
		|| fdwrstr(cso, ui.city) == -1
		|| fdwrstr(cso, ui.birthday) == -1 
		|| fdwrstr(cso, ui.re_que) == -1 
		|| fdwrstr(cso, ui.re_ans) == -1
		|| fdwrstr(cso, ui.locale) == -1
		|| fdwrstr(cso, ui.area.id) == -1
		|| fdwrstr(cso, ui.area.name) == -1
		|| fdwrstr(cso, ui.work.id) == -1
		|| fdwrstr(cso, ui.work.name) == -1
		|| fdwrstr(cso, ui.edu.id) == -1
		|| fdwrstr(cso, ui.edu.name) == -1
		|| fdwrstr(cso, ui.sex.id) == -1
		|| fdwrstr(cso, ui.sex.name) == -1
		|| fdwrite(cso, & icnt, sizeof(icnt)) != sizeof(icnt) )
			throw wr_error();
			
	if( icnt -- ) {
			do {
					if( fdwrstr(cso, ui.ints[icnt].id) == -1 
						|| fdwrstr(cso, ui.ints[icnt].name) == -1 )
								throw wr_error();
			} while( icnt-- );
	}
}

/**
 *
 */
uint8_t ui_ints_ids_get(const string &uid, vector<string> & infos ) {
	infos.clear();

	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT umi.val_id FROM users_info_map_ints AS umi WHERE umi.user_id="
		+pqxx::Quote(uid, false)) );
	
	const char *tmp;
	int tups;
	string inter;
	
	if( (tups=res.size()) == 0 ) return cdbdaemon::err_empty;
	tups--;
	
	do {
			tmp = res[tups][0].c_str();
			if(!tmp) tmp = "";
			inter = tmp;
			infos.push_back(inter);
	} while( tups -- );
	return cdbdaemon::err_no;
}

/**
 *
 */
void cmd_ui_get_by_name1() {
	if( fdrdstr(cso, localeinfo) == -1 
		|| fdrdstr(cso, login) == -1
		|| fdrdstr(cso, dom_id) == -1 )
			throw rd_error();
	
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT ui.id,ui.country,ui.city,ui.birthday,"
		"ui.re_question,ui.re_answer,ui.locale,umw.val_id,ums.val_id,"
		"ume.val_id,uma.val_id FROM users_info as ui,"
		"users_info_map_edu as ume,users_info_map_work as umw,"
		"users_info_map_area as uma,users_info_map_sex as ums where"
		" ui.login="+pqxx::Quote(login, false)+" AND ui.domain_id="
		+pqxx::Quote(dom_id, false)+" AND ui.id=umw.user_id AND "
		"ui.id=ume.user_id and ui.id=ums.user_id"
		" and ui.id=uma.user_id") );
	
	if( res.size() != 1 ) {
			if( fdwrite(cso, &"Z", 1) != 1 ) throw wr_error();
			return;
	}
	 
	const char *tmp;
	int fidx = 0;
	cdbdaemon::user_info ui;
	vector<cdbdaemon::int_info>::size_type icnt;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.id = tmp;
	
	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.country = tmp;
	
	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.city = tmp;
	
	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.birthday = tmp;
	
	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.re_que = tmp;
	
	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.re_ans = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.locale = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.work_id = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.sex_id = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.edu_id = tmp;

	tmp = res[0][fidx++].c_str(); 
	if(!tmp) tmp = "";
	ui.area_id = tmp;

	switch( ui_ints_ids_get(ui.id, ui.ints_id) ) {
	case cdbdaemon::err_no:
	case cdbdaemon::err_empty:
			break;
	default:
   			if( fdwrite( cso, &"E", 1) != 1 ) throw wr_error();
			return;
	}

	icnt = ui.ints_id.size();

	if( fdwrite(cso, &"F", 1) != 1
		|| fdwrstr(cso, ui.id) == -1 
		|| fdwrstr(cso, ui.country) == -1 
		|| fdwrstr(cso, ui.city) == -1
		|| fdwrstr(cso, ui.birthday) == -1 
		|| fdwrstr(cso, ui.re_que) == -1 
		|| fdwrstr(cso, ui.re_ans) == -1
		|| fdwrstr(cso, ui.locale) == -1
		|| fdwrstr(cso, ui.area_id) == -1
		|| fdwrstr(cso, ui.work_id) == -1
		|| fdwrstr(cso, ui.edu_id) == -1
		|| fdwrstr(cso, ui.sex_id) == -1
		|| fdwrite(cso, & icnt, sizeof(icnt)) != sizeof(icnt) )
			throw wr_error();
			
	if( icnt -- ) {
			do {
					if( fdwrstr(cso, ui.ints_id[icnt]) == -1 )
							throw wr_error();
			} while( icnt-- );
	}
}


/**
 *
 */
void cmd_ui_cmp_by_name() {
	if( fdrdstr(cso, localeinfo) == -1 
		|| fdrdstr(cso, login) == -1
		|| fdrdstr(cso, dom_id) == -1
		|| fdrdstr(cso, country) == -1
		|| fdrdstr(cso, city) == -1
		|| fdrdstr(cso, birthday) == -1
		|| fdrdstr(cso, area_id) == -1
		|| fdrdstr(cso, work_id) == -1
		|| fdrdstr(cso, edu_id) == -1
		|| fdrdstr(cso, sex_id) == -1
		|| fdrdstr(cso, re_ans) == -1 )
			throw rd_error();
	
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT 1 FROM users_info as ui,"
		"users_info_map_area as uma,"
		"users_info_map_work as umw,"
		"users_info_map_edu as ume,"
		"users_info_map_sex as ums"
		" WHERE ui.login="+pqxx::Quote(login, false)
		+" AND ui.domain_id="+pqxx::Quote(dom_id, false)
		+" AND ui.country_up=UPPER("+pqxx::Quote(country, false)+")"
		+" AND ui.city_up=UPPER("+pqxx::Quote(city, false)+")"
         " AND birthday="+pqxx::Quote(birthday, false)
		+" AND re_answer=UPPER("+pqxx::Quote(re_ans, false)+")"
		" AND uma.user_id=ui.id AND uma.val_id="+pqxx::Quote(area_id, false)
		+" AND umw.user_id=ui.id AND umw.val_id="+pqxx::Quote(work_id, false)
        +" AND ume.user_id=ui.id AND ume.val_id="+pqxx::Quote(edu_id, false)
        +" AND ums.user_id=ui.id AND ums.val_id="+pqxx::Quote(sex_id, false) ) );

	char resp; 
	if( res.size() == 1 ) resp = 'K';
	else resp = 'Z';

	if( fdwrite(cso, &resp, 1) != 1 ) throw wr_error();
}
/**
 *
 */
void cmd_user_pass_get() {
	if( fdrdstr(cso, domain) == -1 
		|| fdrdstr(cso, login) == -1 )
			throw rd_error();
	
	domain = str2tb(domain);
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT pass FROM \""+domain+"\" WHERE login="
		+pqxx::Quote(login, false)) );

	if( ! res.size() ) {
			if( fdwrite(cso, &"Z", 1) != 1 ) throw wr_error();
			return;
	}

	const char *tmp = res[0][0].c_str();
	if(!tmp) tmp = "";
	if( fdwrite(cso, &"F", 1) != 1
		|| fdwrstr(cso, tmp) == -1 ) throw wr_error();
}

/**
 *
 */
void cmd_ui_re_que_get_by_name() {
	if( fdrdstr(cso, dom_id) == -1 
		|| fdrdstr(cso, login) == -1 ) 
			throw rd_error();

	domain = str2tb(domain);
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT re_question FROM users_info WHERE "
		"login="+pqxx::Quote(login, false)
		+" AND domain_id="+pqxx::Quote(dom_id, false)) );

	if( ! res.size() ) {
			if( fdwrite(cso, &"Z", 1) != 1 ) throw wr_error();
			return;
	}
	
	const char *tmp = res[0][0].c_str();
	if(!tmp) tmp = "";
	if( fdwrite(cso, &"F", 1) != 1
		|| fdwrstr(cso, tmp) == -1 ) throw wr_error();
}

/**
*/
void (*cmd_proc( char cmd )) () {
    switch(cmd) {
    case cdbdaemon::cmd_edu_ls:
        return &cmd_edu_ls;
    case cdbdaemon::cmd_work_ls:
        return &cmd_work_ls;
    case cdbdaemon::cmd_int_ls:
        return &cmd_int_ls;
    case cdbdaemon::cmd_area_ls:
        return &cmd_area_ls;
    case cdbdaemon::cmd_sex_ls:
        return &cmd_sex_ls;
    case cdbdaemon::cmd_dom_ls:
        return &cmd_dom_ls;
    case cdbdaemon::cmd_ui_rm:
        return &cmd_ui_rm;
    case cdbdaemon::cmd_ui_add:
        return &cmd_ui_add;
    case cdbdaemon::cmd_dom_get:
        return &cmd_dom_get;
    case cdbdaemon::cmd_dom_get_by_name:
        return &cmd_dom_get_by_name;
    case cdbdaemon::cmd_ui_get_by_name:
        return &cmd_ui_get_by_name;
    case cdbdaemon::cmd_ui_get_by_name1:
        return &cmd_ui_get_by_name1;
    case cdbdaemon::cmd_ui_cmp_by_name:
        return &cmd_ui_cmp_by_name;
    case cdbdaemon::cmd_user_pass_get:
        return &cmd_user_pass_get;
    case cdbdaemon::cmd_ui_re_que_get_by_name:
        return &cmd_ui_re_que_get_by_name;
    case cdbdaemon::cmd_ui_chg:
        return &cmd_ui_chg;
    default:
        return NULL;
    }
}
/**
*/
char child(int fd)
{
    char cmd;
    void (*run)();

    cso = fd;
    
	switch( fdread(cso, &cmd, 1) ) {
    case 0: return 1;
    case -1: 
            cerr<<"child: read error: "<<strerror(errno)<<endl;
			return 2;
    }
    // process cmd
    if( ! (run = cmd_proc(cmd)) ) {
            cerr << "child: unknown command: " << (int) cmd << endl;
            return 2;
    }
	
	try {
			(*run)();
			return 0;
	} catch( const rd_error & e ) {
			cerr<<"child: exception: "<<e.what()<<endl;
	} catch( const wr_error & e ) {
			cerr<<"child: exception: "<<e.what()<<endl;
	} catch( const exception & e ) {
			cerr<<"child: exception: "<<e.what()<<endl;
			if( fdwrite(cso, &"E", 1) != 1 ) cerr<<"child: write error: "<<strerror(errno)<<endl;
	}
    
	return 2;
}
