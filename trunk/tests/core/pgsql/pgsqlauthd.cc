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
#include "../../../core/auth.hpp"

#include <getlines.hpp>
#include <split.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>

#include <coss/CosNaming.h>

#include <memory>
#include <sstream>
#include <map>
#include <fstream>

using namespace boost::unit_test_framework;
using boost::lexical_cast;

#define std_try { try
#define std_catch catch( vq::null_error & e ) { \
	BOOST_ERROR("null_error"); \
} catch( vq::db_error & e ) { \
	BOOST_ERROR(e.what); \
} catch( vq::except & e ) { \
	BOOST_ERROR(e.what); \
} }
	
#define IVQ_ERROR_EQUAL(eptr, ecmp) \
do { \
	::vq::ivq::error_var ev(eptr); \
	::vq::ivq::err_code ec = ecmp; \
	BOOST_CHECK_EQUAL(ev->ec, ec); \
	if( ec != ev->ec ) \
		BOOST_ERROR(error2str(ev)); \
} while(0)

/**
 * 
 */
void et_vq_null_error( const vq::null_error &e ) {
	BOOST_ERROR("null");
}

/**
 * 
 */
void et_vq_except( const vq::except &e ) {
	BOOST_ERROR(e.what);
}

/**
 * 
 */
void et_vq_db_error( const vq::db_error &e ) {
	BOOST_ERROR(e.what);
}

/**
 * 
 */
void et_corba_exception( const CORBA::Exception & e ) {
	std::ostringstream os;
	e._print(os);
	BOOST_ERROR(os.str());
}

/**
 *
 */
std::string error2str( const vq::ivq::error * err ) {
	std::ostringstream os;
	os<<"error: "<<err->what<<" in "<<err->file<<" at "<<err->line;
	return os.str();
}


/**
 *
 */ 
struct auth_test {
		vq::iauth_var auth;
		vq::ivq::error_var err;
		int ac;
		char **av;
		
		auth_test( int ac, char **av ) : ac(ac), av(av) {
		}

		void init() {
			CORBA::ORB_var orb = CORBA::ORB_init(this->ac, this->av);
			CORBA::Object_var nsobj = 
					orb->resolve_initial_references ("NameService");
			
			CosNaming::NamingContext_var nc =
					CosNaming::NamingContext::_narrow (nsobj);

			BOOST_REQUIRE( !CORBA::is_nil(nc) );

			CosNaming::Name name;
			name.length (1);
			name[0].id = CORBA::string_dup ("vq::iauth");
			name[0].kind = CORBA::string_dup ("");

			CORBA::Object_var obj;
			try {
					obj = nc->resolve (name);
			} catch (CosNaming::NamingContext::NotFound &exc) {
					BOOST_FAIL("NotFound exception.");
			} catch (CosNaming::NamingContext::CannotProceed &exc) {
					BOOST_FAIL("CannotProceed exception.");
			} catch (CosNaming::NamingContext::InvalidName &exc) {
					BOOST_FAIL("InvalidName exception.");
			}

			auth = vq::iauth::_narrow(obj);

			BOOST_REQUIRE( !CORBA::is_nil(auth) );
		}
	
		/**
		 *
		 */
		void test_dom_rm(const char * dom) {
			CORBA::String_var dom_id;
			err = auth->dom_id(dom, dom_id);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no );
			err = auth->dom_rm(dom_id);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
		}

		/**
		 *
		 */
		void test_dom_user_add( const char * dom, CORBA::String_var & dom_id ) {
			err = auth->dom_add(dom, dom_id);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
		
			vq::iauth::auth_info ai;
			ai.id_domain = CORBA::string_dup(dom_id);
			ai.pass = CORBA::string_dup("pass");
			ai.dir = CORBA::string_dup("dir");
			ai.login = CORBA::string_dup(dom);
			ai.flags = 0;
		
			err = auth->user_add(ai, FALSE);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
		}

		/**
		 * Add domain, 
		 * add users, 
		 * remove uses, 
		 * remove domain.
		 */
		void case1() {
			const char *dom = "case1.pl";
			const char *users[] = { "s", "asdasd", "ZXCXZCZXC", "zxcxzADSDF", 
					"vxcvcxvxcvxvcv" };
			unsigned users_cnt= sizeof(users)/sizeof(*users);
		
			CORBA::String_var dom_id;
			err = auth->dom_add(dom, dom_id);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);

			vq::iauth::auth_info ai;
			ai.id_domain = CORBA::string_dup(dom_id);
			ai.pass = CORBA::string_dup("pass");
			ai.dir = CORBA::string_dup("dir");

			for( unsigned i=0; i<users_cnt; ++i ) {
					ai.login = CORBA::string_dup(users[i]);
					err = auth->user_add(ai, FALSE);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);

					err = auth->user_ex(ai.id_domain, ai.login);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			}

			for( unsigned i=0; i<users_cnt; ++i ) {
					err = auth->user_rm(dom_id, users[i]);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			}
			test_dom_rm(dom);
		}

		/**
		 * 1. Adding domain, adding the same domain again (err_exists),
		 * 2. getting id from domain's name
		 * 3. getting id from domain's name given in upper-lower case (mixed)
		 * 4. removing domain, removing again (no errors).
		 */
		void case2() {
			const char *dom = "test.pl";
			const char *dom_mixed = "TeST.pl";
			CORBA::String_var dom_id;
			CORBA::String_var dom_id1;

			// 1.
			err = auth->dom_add(dom, dom_id);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			err = auth->dom_add(dom, dom_id1);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_exists);

			// 2.
			CORBA::String_var dom_id2;
			err = auth->dom_id(dom, dom_id2);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			BOOST_CHECK( *dom_id2 && atoi(dom_id2) > 0);

			// 3.
			CORBA::String_var dom_id3;
			err = auth->dom_id(dom_mixed, dom_id3);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			BOOST_CHECK( !strcmp(dom_id2, dom_id3) );

			// 4.
			test_dom_rm(dom_mixed);
		}

		/**
		 * trying to get ids of not existing domains
		 */
		void case3() {
			const char *doms[] = {
					"",
					"nosuch.domain",
					".",
					"\"",
					"----",
					"'",
					";;''"
			};
			unsigned doms_cnt = sizeof(doms)/sizeof(*doms);
			if( doms_cnt-- ) {
				CORBA::String_var id;
				do {
						err = auth->dom_id(doms[doms_cnt], id);
						BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_noent);
						BOOST_CHECK(!*id);
				} while(doms_cnt --);
			}
		}

		/**
		 * Add domain, 
		 * add users, 
		 * remove domain.
		 */
		void case4() {
			const char * dom = "case4.pl";
			CORBA::String_var dom_id;

			std_try {
					const char *users[] = { "s", "asdasd", "vxcvcxvxcvxvcv" };
					unsigned users_cnt= sizeof(users)/sizeof(*users);
					
					err = auth->dom_add(dom, dom_id);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);

					vq::iauth::auth_info ai;
					ai.id_domain = CORBA::string_dup(dom_id);
					ai.pass = CORBA::string_dup("pass");
					ai.dir = CORBA::string_dup("dir");

					for( unsigned i=0; i<users_cnt; ++i ) {
							ai.login = CORBA::string_dup(users[i]);
								
							err = auth->user_add(ai, FALSE);
							BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
					}
			} std_catch
			test_dom_rm(dom);
		}

		/**
		 * Get all banned emails, remove all of them
		 * 
		 */
		void case5() {
			CORBA::String_var dom_id;
			const char * dom = "case5.pl";

			std_try {
					::vq::iauth::email_banned_list_var ebs;
					err = auth->eb_ls(ebs);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
					CORBA::ULong s = ebs->length();
					for( CORBA::ULong i=0; i<s; ++i ) {
							err = auth->eb_rm(ebs[i].re_domain, ebs[i].re_login);
							BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
					}
					err = auth->dom_add(dom, dom_id);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			
					err = auth->eb_add(static_cast<const char *>(".*"),
						static_cast<const char *>("root"));
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
					err = auth->eb_add(static_cast<const char *>("case5"),
						static_cast<const char *>(".*"));
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
					err = auth->eb_ls(ebs);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
					BOOST_CHECK_EQUAL(ebs->length(), 2U);
					if( !strcmp(ebs[0U].re_domain, ".*") ) {
							BOOST_CHECK(!strcmp(ebs[1U].re_domain, "case5"));
							BOOST_CHECK(!strcmp(ebs[1U].re_login, ".*"));
							BOOST_CHECK(!strcmp(ebs[0U].re_login, "root"));
					} else {
							BOOST_CHECK(!strcmp(ebs[0U].re_domain, "case5"));
							BOOST_CHECK(!strcmp(ebs[0U].re_login, ".*"));
							BOOST_CHECK(!strcmp(ebs[1U].re_domain, ".*"));
							BOOST_CHECK(!strcmp(ebs[1U].re_login, "root"));
					}

					vq::iauth::auth_info ai;
					ai.id_domain = CORBA::string_dup(dom_id);
					ai.pass = CORBA::string_dup("pass");
					ai.dir = CORBA::string_dup("dir");
					ai.login = CORBA::string_dup("aroote");
					ai.flags = 0;
					
					err = auth->user_add(ai, TRUE);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_user_inv);
					err = auth->user_add(ai, TRUE);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_user_inv);
					err = auth->user_add(ai, FALSE);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			} std_catch
			test_dom_rm(dom);
		}

		/**
		 * Add domain and user if they don't exists
		 * Change password for user, test if it is changed
		 */
		void case6() {
			const char * dom = "case6.pl";
			CORBA::String_var dom_id;
			err = auth->dom_id(dom, dom_id);
			if( err->ec != vq::ivq::err_no ) {
					err = auth->dom_add(dom, dom_id);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			}

			vq::iauth::auth_info ai;
			ai.id_domain = CORBA::string_dup(dom_id);
			ai.pass = CORBA::string_dup("pass");
			ai.dir = CORBA::string_dup("dir");
			ai.login = CORBA::string_dup(dom);
			ai.flags = 0;

			err = auth->user_ex(dom_id, ai.login);
			if( err->ec == vq::ivq::err_noent ) {
					err = auth->user_add(ai, FALSE);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			}
			std::string now(boost::posix_time::to_iso_string(
				boost::posix_time::second_clock::local_time()));
			err = auth->user_pass(dom_id, ai.login, now.c_str());
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no );

			vq::iauth::auth_info aicur;
			aicur.id_domain = ai.id_domain;
			aicur.login = ai.login;
			aicur.pass = now.c_str();
			err = auth->user_auth(aicur);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no );
			BOOST_CHECK_EQUAL(aicur.flags, 0);

			aicur.pass = CORBA::string_dup("");
			err = auth->user_auth(aicur);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_pass_inv);
		}

		/**
		 * add regex aliases for domain created in case6
		 * get list of regex aliases for domain created in case6
		 * get list of regex aliases for not existing domain
		 * remove regex aliases by pair: id_domain+re_alias
		 * remove regex aliases by id_domain
		 */
		void case7() {
			const char * dom = "case6.pl";
			const char * reas[] = {
				"test",
				"^test.*",
				"^oloo$"
			};
			unsigned reas_cnt = sizeof(reas)/sizeof(*reas);
			CORBA::String_var dom_id;
			err = auth->dom_id(dom, dom_id);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			BOOST_REQUIRE(*dom_id);
			for( unsigned i=0; i<reas_cnt; ++i ) {
					err = auth->dra_add(dom_id, reas[i]);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			}
			vq::iauth::string_list_var from_db, from_db1;
			err = auth->dra_ls_by_dom(dom_id, from_db);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			BOOST_CHECK_EQUAL(from_db->length(), reas_cnt);
			// need to compare all of them
			std::map<std::string, char> in_db;
			for( unsigned i=0, s=from_db->length(); i<s; ++i ) {
					in_db[static_cast<const char *>(from_db[i])] = 1;
			}
			for( unsigned i=0; i<reas_cnt; ++i ) {
					BOOST_CHECK_EQUAL(in_db[reas[i]], 1);
			}
		
			err = auth->dra_ls_by_dom(static_cast<const char *>("-1"), from_db1);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no );
			BOOST_CHECK_EQUAL(from_db1->length(), 0U);

			err = auth->dra_rm(dom_id, reas[0]);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no );
			err = auth->dra_ls_by_dom(dom_id, from_db);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			BOOST_CHECK_EQUAL(from_db->length(), reas_cnt-1);
		
			err = auth->dra_rm_by_dom(dom_id);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no );
			err = auth->dra_ls_by_dom(dom_id, from_db);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			BOOST_CHECK_EQUAL(from_db->length(), 0U);
		}

		/**
		 * Change default quota values for users,
		 * add user,
		 * check quotas for user,
		 * change quotas for user,
		 * check quotas for user,
		 * delete user,
		 * revert to default quotas for new users
		 */
		void case8() {
			const char * dom = "case6.pl";
			CORBA::String_var dom_id;

			std_try {
					err = auth->dom_id(dom, dom_id);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
					BOOST_REQUIRE(*dom_id);
		
					::vq::iauth::quota_type bytes_max, files_max;
					::vq::iauth::quota_type ubytes, ufiles;
					err = auth->qt_user_def_get(dom_id, 
						bytes_max, files_max);
					BOOST_CHECK_EQUAL(err->ec, ::vq::ivq::err_no );
					BOOST_CHECK_EQUAL(bytes_max, 0U);
					BOOST_CHECK_EQUAL(files_max, 0U);
		
					// changing default quota for users
					bytes_max = 1000;
					files_max = 500;
					err = auth->qt_user_def_set(dom_id,
						bytes_max, files_max);
					BOOST_CHECK_EQUAL(err->ec, ::vq::ivq::err_no);
					err = auth->qt_user_def_get(dom_id,
						ubytes, ufiles);
					BOOST_CHECK_EQUAL(err->ec, ::vq::ivq::err_no );
					BOOST_CHECK_EQUAL(files_max, ufiles);
					BOOST_CHECK_EQUAL(bytes_max, ubytes);
		
					// adding user
					vq::iauth::auth_info ai;
					ai.id_domain = CORBA::string_dup(dom_id);
					ai.pass = CORBA::string_dup("pass");
					ai.dir = CORBA::string_dup("dir");
					ai.login = CORBA::string_dup("case8");
					ai.flags = 0;
		
					err = auth->user_add(ai, FALSE);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
		
					// check quota for created user
					err = auth->qt_user_get(dom_id, ai.login,
						ubytes, ufiles);
					BOOST_CHECK_EQUAL(err->ec, ::vq::ivq::err_no );
					BOOST_CHECK_EQUAL(ubytes, bytes_max);
					BOOST_CHECK_EQUAL(ufiles, files_max);
		
					// change quotas for user
					files_max += 501;
					bytes_max += 501;
					err = auth->qt_user_set(dom_id, ai.login,
						bytes_max, files_max);
					BOOST_CHECK_EQUAL(err->ec, ::vq::ivq::err_no );
			
					// check quotas
					err = auth->qt_user_get(dom_id, ai.login,
						ubytes, ufiles);
					BOOST_CHECK_EQUAL(err->ec, ::vq::ivq::err_no );
					BOOST_CHECK_EQUAL(ubytes, bytes_max);
					BOOST_CHECK_EQUAL(ufiles, files_max);
			} std_catch

			// remove user
			err = auth->user_rm(dom_id, static_cast<const char *>("case8"));
			BOOST_CHECK_EQUAL(err->ec, ::vq::ivq::err_no );

			// revert to default values
			err = auth->dom_id(dom, dom_id);
			BOOST_CHECK_EQUAL(err->ec, ::vq::ivq::err_no );
			err = auth->qt_user_def_set(dom_id, 0U, 0U);
			BOOST_CHECK_EQUAL(err->ec, ::vq::ivq::err_no );
		}

		/**
		 * Add domain. Create users as specified in configuration file.
		 * For each users create many configuration entries as specified
		 * in configuration files. When adding entries remeber their id.
		 * Remove all entries by id. Get all entries for ids. Should return
		 * err_noent. In the mean-time
		 * call user_conf_type_has, user_conf_type_cnt (when user, pfix has entries, and
		 * after removing them).
		 */
		void case9() {
			const char * dom = "case9.pl";
			CORBA::String_var dom_id;
			std_try {
					typedef std::deque< std::string > string_array;
					test_dom_user_add(dom, dom_id);

					string_array users, pfixs;
					std::ifstream ifs("data/case9/users");
					BOOST_REQUIRE(sys::getlines<std::string>(ifs, users));
					ifs.close();
					ifs.clear();
					ifs.open("data/case9/pfixs");
					BOOST_REQUIRE(sys::getlines<std::string>(ifs, pfixs));
					
					string_array::const_iterator ubeg, uend, pbeg, pend;
					::vq::ivq::user_conf_info ui;
					::vq::ivq::auth_info ai;
					ui.type = ::vq::ivq::ut_redir;
					typedef std::vector<CORBA::String_var> id_array;
					id_array ids;
					ids.reserve(users.size()*pfixs.size());
					for( ubeg=users.begin(), uend=users.end(); 
								ubeg!=uend; ++ubeg) {

							ai.id_domain = dom_id;
							ai.pass = dom_id;
							ai.flags = 0;
							ai.login = ubeg->c_str(); 
							IVQ_ERROR_EQUAL(auth->user_add(ai, FALSE),
								::vq::ivq::err_no);

							for( pbeg=pfixs.begin(), pend=pfixs.end();
										pbeg!=pend; ++pbeg ) {

									ui.val = pbeg->c_str();
									ui.id_conf = static_cast<const char *>("");
									IVQ_ERROR_EQUAL(auth->user_conf_add(dom_id,
										ubeg->c_str(), pbeg->c_str(), ui),
										::vq::ivq::err_no);
									BOOST_CHECK(*ui.id_conf);
									ids.push_back(ui.id_conf);

									// Check if user,pfix has entries for
									// type ui.type (should have them)
									IVQ_ERROR_EQUAL(auth->user_conf_type_has(
										dom_id, ubeg->c_str(), pbeg->c_str(), 
										ui.type), ::vq::ivq::err_no);

									// Check number of entries for user,pfix
									CORBA::ULong cnt;
									IVQ_ERROR_EQUAL(auth->user_conf_type_cnt(
										dom_id, ubeg->c_str(), pbeg->c_str(), 
										ui.type, cnt), ::vq::ivq::err_no);
									BOOST_CHECK_EQUAL( cnt, 1U );

									// Get entry and compare
									::vq::ivq::user_conf_info uicmp;
									uicmp.id_conf = ui.id_conf;
									IVQ_ERROR_EQUAL(auth->user_conf_get(uicmp),
										::vq::ivq::err_no);
									BOOST_CHECK_EQUAL(uicmp.id_conf, ui.id_conf);
									BOOST_CHECK_EQUAL(uicmp.type, ui.type);
									BOOST_CHECK_EQUAL(uicmp.val, ui.val);
							}
					}
					// remove entries
					for( id_array::const_iterator beg=ids.begin(), end=ids.end();
								beg!=end; ++beg ) {
							IVQ_ERROR_EQUAL(auth->user_conf_rm(*beg),
								::vq::ivq::err_no);
					}
					// get entries
					for( id_array::const_iterator beg=ids.begin(), end=ids.end();
								beg!=end; ++beg ) {
							ui.id_conf = *beg;
							IVQ_ERROR_EQUAL(auth->user_conf_get(ui),
								::vq::ivq::err_noent);
					}
			} std_catch
			test_dom_rm(dom);
		}

		/**
		 * Add domain. Create users as specified in configuration file.
		 * For each users create many configuration entries as specified
		 * in configuration file. For each pair user,pfix call user_conf_ls
		 * and check whether it returns all entries as in configuration
		 * file. Also for each pair call user_conf_ls_by_type and check results.
		 * Remove domain.
		 */
		void case10() {
			const char * dom = "case10.pl";
			std_try {
					typedef std::deque< std::string > string_array;
					CORBA::String_var dom_id;
					test_dom_user_add(dom, dom_id);

					string_array conf;
					std::ifstream ifs("data/case10/conf");
					BOOST_REQUIRE(sys::getlines<std::string>(ifs, conf));
					ifs.close();

					::vq::ivq::auth_info ai;
					ai.id_domain = dom_id;
					ai.pass = static_cast<const char *>("asdasd");
					ai.flags = 0;

					string_array::const_iterator bc, be, fc, fe, pfix;
					string_array fields, uia;
					::vq::ivq::user_conf_info ui;
					
					for( bc=conf.begin(), be=conf.end(); bc!=be; ++bc ) {
							if( '#' == (*bc)[0] ) continue;
							fields = text::split(*bc, " ");
							fc = fields.begin();
							fe = fields.end();
							BOOST_REQUIRE(fc != fe);
							ai.login = fc->c_str();
							
							err = auth->user_add(ai, FALSE);
							BOOST_CHECK( err->ec == ::vq::ivq::err_no 
								|| err->ec == ::vq::ivq::err_exists );
							++fc;
							if( fc == fe ) continue;
							
							for( pfix=fc, ++fc; fc!=fe; ++fc )  {
									uia = text::split(*fc, ",");
									BOOST_REQUIRE(uia.size() == 2);
									ui.type = lexical_cast<CORBA::UShort>
										(uia[0].c_str());
									ui.val = uia[1].c_str();
									IVQ_ERROR_EQUAL( auth->user_conf_add(dom_id,
										ai.login, pfix->c_str(), ui), 
										::vq::ivq::err_no);
							}
					}

					for( bc=conf.begin(), be=conf.end(); bc!=be; ++bc ) {
							if( '#' == (*bc)[0] ) continue;
							fields = text::split(*bc, " ");
							fc = fields.begin();
							fe = fields.end();
							ai.login = fc->c_str();
							
							++fc;
							if( fc == fe ) continue;

							pfix = fc++;

							::vq::ivq::user_conf_info_list_var uis;
							IVQ_ERROR_EQUAL(auth->user_conf_ls(dom_id,
								ai.login, pfix->c_str(), uis ),
								::vq::ivq::err_no );

							BOOST_CHECK_EQUAL(uis->length(), fields.size()-2);
						
							for( ; fc!=fe; ++fc )  {
									uia = text::split(*fc, ",");
									ui.type = lexical_cast<CORBA::UShort>
										(uia[0].c_str());
									ui.val = uia[1].c_str();
									bool has = false;
									CORBA::ULong type_cnt=0;
									for( CORBA::ULong i=0, s=uis->length();
												i<s; ++i ) {
											if( uis[i].type == ui.type ) {
													++type_cnt;
													if(  uis[i].val == ui.val )
															has = true;
											}
									}
									BOOST_CHECK(has);

									::vq::ivq::user_conf_info_list_var uistype;
									IVQ_ERROR_EQUAL(auth->user_conf_ls_by_type(dom_id,
										ai.login, pfix->c_str(), ui.type, uistype ),
										::vq::ivq::err_no );

									BOOST_CHECK_EQUAL(uistype->length(), type_cnt);
									has = false;
									for( CORBA::ULong i=0, s=uistype->length();
												i<s; ++i ) {
											if( uistype[i].type == ui.type 
												&& uistype[i].val == ui.val ) {
													has = true;
													break;
											}
									}
									BOOST_CHECK(has);
							}
					}
			} std_catch
			test_dom_rm(dom);
		}

		/**
		 * Try to change entry which doesn't exist. 
		 */
		void case11() {
			::vq::ivq::user_conf_info uci;
			uci.id_conf = static_cast<const char *>("123123123");
			uci.type = 12;
			uci.val = static_cast<const char *>("asd");
			IVQ_ERROR_EQUAL(auth->user_conf_rep(uci), ::vq::ivq::err_noent);
		}

		/**
		 * Add domain and user. Add configuration
		 *
		 */
		void case12() {
			const char * dom = "case12.pl";
			std_try {
					CORBA::String_var dom_id;
					test_dom_user_add(dom, dom_id);

					::vq::ivq::auth_info ai;
					ai.id_domain = dom_id;
					ai.pass = static_cast<const char *>("asdasd");
					ai.flags = 0;
					ai.login = static_cast<const char *>("test");
					IVQ_ERROR_EQUAL(auth->user_add(ai, FALSE), ::vq::ivq::err_no);

					typedef std::vector< ::vq::ivq::user_conf_info > uci_array; 
					uci_array ucis;
					::vq::ivq::user_conf_info uci;
					uci.type = 1;
					uci.val = static_cast<const char *>("1");
					ucis.push_back(uci);
					++uci.type;
					ucis.push_back(uci);
					ucis.push_back(uci);
					++uci.type;
					ucis.push_back(uci);
					ucis.push_back(uci);
					uci_array::iterator ub, ue;
					
					for( ub=ucis.begin(), ue=ucis.end(); ub!=ue; ++ub ) {
							IVQ_ERROR_EQUAL(auth->user_conf_add(dom_id,
								ai.login, static_cast<const char *>(""),
								*ub ), ::vq::ivq::err_no );
					}
					for( ub=ucis.begin(), ue=ucis.end(); ub!=ue; ++ub ) {
							++ub->type;
							ub->val = static_cast<const char *>("2");
							IVQ_ERROR_EQUAL(auth->user_conf_rep(
								*ub ), ::vq::ivq::err_no );
					}
					for( ub=ucis.begin(), ue=ucis.end(); ub!=ue; ++ub ) {
							uci.id_conf = ub->id_conf;
							IVQ_ERROR_EQUAL(auth->user_conf_get(
								uci ), ::vq::ivq::err_no );
							BOOST_CHECK_EQUAL(uci.type, ub->type);
							BOOST_CHECK_EQUAL(uci.val, ub->val);
					}
					::vq::ivq::size_type cnt;
					for( ub=ucis.begin(), ue=ucis.end(); ub!=ue; ++ub ) {
							IVQ_ERROR_EQUAL(auth->user_conf_rm_by_type(dom_id,
								ai.login, static_cast<const char *>(""),
								ub->type ), ::vq::ivq::err_no );
							IVQ_ERROR_EQUAL(auth->user_conf_type_cnt(dom_id,
								ai.login, static_cast<const char *>(""),
								ub->type, cnt ), ::vq::ivq::err_no );
							BOOST_CHECK_EQUAL(cnt, 0U);
					}

			} std_catch
			test_dom_rm(dom);
		}

		/**
		 * Try to remove configuration for non-existing user.
		 * Try to remove configuration of type which user has not any item.
		 */
		void case13() {
			IVQ_ERROR_EQUAL(auth->user_conf_rm_by_type(
				static_cast<const char *>("123123123"), 
				static_cast<const char *>("login"),
				static_cast<const char *>(""),
				12), ::vq::ivq::err_no);
			const char * dom = "case13.pl";
			std_try {
					CORBA::String_var dom_id;
					test_dom_user_add(dom, dom_id);

					IVQ_ERROR_EQUAL(auth->user_conf_rm_by_type(dom_id,
						static_cast<const char *>("login"),
						static_cast<const char *>(""),
						12), ::vq::ivq::err_no);

					::vq::ivq::auth_info ai;
					ai.id_domain = dom_id;
					ai.pass = static_cast<const char *>("asdasd");
					ai.flags = 0;
					ai.login = static_cast<const char *>("test");
					IVQ_ERROR_EQUAL(auth->user_add(ai, FALSE), ::vq::ivq::err_no);

					IVQ_ERROR_EQUAL(auth->user_conf_rm_by_type(dom_id,
						ai.login, static_cast<const char *>(""),
						12), ::vq::ivq::err_no);

					::vq::ivq::user_conf_info uci;
					uci.type = 15;
					uci.val = static_cast<const char *>("asd");
					IVQ_ERROR_EQUAL(auth->user_conf_add(dom_id,
						ai.login, static_cast<const char *>(""), 
						uci), ::vq::ivq::err_no);

					IVQ_ERROR_EQUAL(auth->user_conf_rm_by_type(dom_id,
						ai.login, static_cast<const char *>(""),
						12), ::vq::ivq::err_no);

					::vq::ivq::size_type cnt=0;
					IVQ_ERROR_EQUAL(auth->user_conf_type_cnt(dom_id,
						ai.login, static_cast<const char *>(""),
						12, cnt ), ::vq::ivq::err_no);
					BOOST_CHECK_EQUAL(cnt, 0U);

					IVQ_ERROR_EQUAL(auth->user_conf_type_cnt(dom_id,
						ai.login, static_cast<const char *>(""),
						uci.type, cnt ), ::vq::ivq::err_no);
					BOOST_CHECK_EQUAL(cnt, 1U);
			} std_catch
			test_dom_rm(dom);
		}
}; // struct auth_test

/**
 * 
 */
struct auth_test_suite : test_suite {
		auth_test_suite(int ac, char *av[]) 
				: test_suite("pgsqlauthd tests") {
			boost::shared_ptr<auth_test> test(new auth_test(ac, av));

			test_case * ts_init = BOOST_CLASS_TEST_CASE( 
				&auth_test::init, test );
			add(ts_init);

			test_case * ts_case1 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case1, test );
			ts_case1->depends_on(ts_init);
			add(ts_case1);
			
			test_case * ts_case2 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case2, test );
			ts_case2->depends_on(ts_init);
			add(ts_case2);

			test_case * ts_case3 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case3, test );
			ts_case3->depends_on(ts_init);
			add(ts_case3);

			test_case * ts_case4 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case4, test );
			ts_case4->depends_on(ts_init);
			add(ts_case4);

			test_case * ts_case5 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case5, test );
			ts_case5->depends_on(ts_init);
			add(ts_case5);

			test_case * ts_case6 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case6, test );
			ts_case6->depends_on(ts_init);
			add(ts_case6);

			test_case * ts_case7 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case7, test );
			ts_case7->depends_on(ts_init);
			add(ts_case7);

			test_case * ts_case8 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case8, test );
			ts_case8->depends_on(ts_init);
			add(ts_case8);

			test_case * ts_case9 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case9, test );
			ts_case9->depends_on(ts_init);
			add(ts_case9);

			test_case * ts_case10 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case10, test );
			ts_case10->depends_on(ts_init);
			add(ts_case10);

			test_case * ts_case11 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case11, test );
			ts_case11->depends_on(ts_init);
			add(ts_case11);

			test_case * ts_case12 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case12, test );
			ts_case12->depends_on(ts_init);
			add(ts_case12);

			test_case * ts_case13 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case13, test );
			ts_case13->depends_on(ts_init);
			add(ts_case13);
		}
};

/**
 *
 */
test_suite* init_unit_test_suite( int ac, char* av[] ) {
	std::auto_ptr<test_suite> test(BOOST_TEST_SUITE("pgsqlauthd tests"));

	register_exception_translator<vq::null_error>( &et_vq_null_error );
	register_exception_translator<vq::db_error>( &et_vq_db_error );
	register_exception_translator<vq::except>( &et_vq_except );
	register_exception_translator<CORBA::Exception>( &et_corba_exception );

	test->add(new auth_test_suite(ac, av));

    return test.release();
}
 
