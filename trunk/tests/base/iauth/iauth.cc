/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2005 Pawel Niewiadomski (new@foo-baz.com)

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
#include "../etrans.hpp"
#include "iauth_common.hpp"
#include "iauth_user_conf.hpp"
#include "iauth_dom_alias.hpp"
#include "iauth_dom_ls.hpp"

#include <auth.hpp>
#include <split.hpp>

#include <import_export.h>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>

#include <memory>
#include <sstream>
#include <map>
#include <fstream>

using namespace boost::unit_test_framework;
using boost::lexical_cast;

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
			
			BOOST_REQUIRE(ac>1);

			CORBA::Object_var obj;
			try {
					obj = corbautil::importObjRef(orb, av[1]);
			} catch ( corbautil::ImportExportException & e ) {
					BOOST_ERROR(e.msg);
			}

			auth = vq::iauth::_narrow(obj);

			BOOST_REQUIRE( !CORBA::is_nil(auth) );
		}
	
		/**
		 *
		 */
		void test_dom_rm(const char * dom) {
			CORBA::String_var dom_id = CORBA::string_dup("");
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
		
			vq::iauth::user_info ai;
			ai.id_domain = CORBA::string_dup(dom_id);
			ai.pass = CORBA::string_dup("pass");
			ai.dir = CORBA::string_dup("dir");
			ai.login = CORBA::string_dup(dom);
			ai.flags = 0;
		
			err = auth->user_add(ai, 0);
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
		
			CORBA::String_var dom_id = CORBA::string_dup("");
			err = auth->dom_add(dom, dom_id);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);

			vq::iauth::user_info ai;
			ai.id_domain = CORBA::string_dup(dom_id);
			ai.pass = CORBA::string_dup("pass");
			ai.dir = CORBA::string_dup("dir");

			for( unsigned i=0; i<users_cnt; ++i ) {
					ai.login = CORBA::string_dup(users[i]);
					err = auth->user_add(ai, 0);
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
			CORBA::String_var dom_id = CORBA::string_dup("");
			CORBA::String_var dom_id1 = CORBA::string_dup("");

			// 1.
			err = auth->dom_add(dom, dom_id);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			err = auth->dom_add(dom, dom_id1);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_exists);

			// 2.
			CORBA::String_var dom_id2 = CORBA::string_dup("");
			err = auth->dom_id(dom, dom_id2);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			BOOST_CHECK( *dom_id2 && atoi(dom_id2) > 0);

			// 3.
			CORBA::String_var dom_id3 = CORBA::string_dup("");
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
				CORBA::String_var id = CORBA::string_dup("");
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
			CORBA::String_var dom_id = CORBA::string_dup("");

			std_try {
					const char *users[] = { "s", "asdasd", "vxcvcxvxcvxvcv" };
					unsigned users_cnt= sizeof(users)/sizeof(*users);
					
					err = auth->dom_add(dom, dom_id);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);

					vq::iauth::user_info ai;
					ai.id_domain = CORBA::string_dup(dom_id);
					ai.pass = CORBA::string_dup("pass");
					ai.dir = CORBA::string_dup("dir");

					for( unsigned i=0; i<users_cnt; ++i ) {
							ai.login = CORBA::string_dup(users[i]);
								
							err = auth->user_add(ai, 0);
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
			CORBA::String_var dom_id = CORBA::string_dup("");
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

					vq::iauth::user_info ai;
					ai.id_domain = CORBA::string_dup(dom_id);
					ai.pass = CORBA::string_dup("pass");
					ai.dir = CORBA::string_dup("dir");
					ai.login = CORBA::string_dup("aroote");
					ai.flags = 0;
					
					err = auth->user_add(ai, 1);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_user_inv);
					err = auth->user_add(ai, 1);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_user_inv);
					err = auth->user_add(ai, 0);
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
			CORBA::String_var dom_id = CORBA::string_dup("");
			err = auth->dom_id(dom, dom_id);
			if( err->ec != vq::ivq::err_no ) {
					err = auth->dom_add(dom, dom_id);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			}

			vq::iauth::user_info ai;
			ai.id_domain = CORBA::string_dup(dom_id);
			ai.pass = CORBA::string_dup("pass");
			ai.dir = CORBA::string_dup("dir");
			ai.login = CORBA::string_dup(dom);
			ai.flags = 0;

			err = auth->user_ex(dom_id, ai.login);
			if( err->ec == vq::ivq::err_noent ) {
					err = auth->user_add(ai, 0);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			}
			std::string now(boost::posix_time::to_iso_string(
				boost::posix_time::second_clock::local_time()));
			err = auth->user_pass(dom_id, ai.login, now.c_str());
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no );

			vq::iauth::user_info aicur;
			aicur.id_domain = ai.id_domain;
			aicur.login = ai.login;
			err = auth->user_get(aicur);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no );
			BOOST_CHECK_EQUAL(now, static_cast<const char *>(aicur.pass));
			BOOST_CHECK(!strcmp(aicur.dir, ""));
			BOOST_CHECK_EQUAL(aicur.flags, 0U);
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
			CORBA::String_var dom_id = CORBA::string_dup("");

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
					vq::iauth::user_info ai;
					ai.id_domain = CORBA::string_dup(dom_id);
					ai.pass = CORBA::string_dup("pass");
					ai.dir = CORBA::string_dup("dir");
					ai.login = CORBA::string_dup("case8");
					ai.flags = 0;
		
					err = auth->user_add(ai, 0);
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

}; // struct auth_test

/**
 * 
 */
struct auth_test_suite : test_suite {
		struct auth_wrap {
				::vq::iauth_var & auth;
				
				auth_wrap( ::vq::iauth_var & a ) : auth(a) {}
				
				::vq::ivq::error * user_conf_rm(const char *,
						const char *, const char *, const char * id ) {
					return auth->user_conf_rm(id);
				}

				::vq::ivq::error * user_conf_rep( const char *, const char *,
						const char *, const ::vq::ivq::user_conf_info & ui ) {
					return auth->user_conf_rep(ui);
				}
		};
		
		typedef user_conf_test<vq::iauth_var, auth_wrap> obj_user_conf_test;
		typedef da_dip_test<vq::iauth_var> obj_dom_alias_test;
		typedef dom_ls_test<vq::iauth_var> obj_dom_ls_test;

		boost::shared_ptr<auth_test> test;
		boost::shared_ptr< obj_user_conf_test > uc_test;
		boost::shared_ptr< obj_dom_alias_test > da_test;
		boost::shared_ptr< obj_dom_ls_test > dom_ls_test;

		auth_test_suite(int ac, char *av[]) 
				: test_suite("pgsqlauthd tests"), 
				test(new auth_test(ac, av)),
				uc_test(new obj_user_conf_test(test->auth)),
				da_test(new obj_dom_alias_test(test->auth)),
				dom_ls_test(new obj_dom_ls_test(test->auth)) {

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

			// da_dip_test
			{
					test_case * ts_case4 = BOOST_CLASS_TEST_CASE( 
						&obj_dom_alias_test::case4, da_test );
					ts_case4->depends_on(ts_init);
					add(ts_case4);

					test_case * ts_case5 = BOOST_CLASS_TEST_CASE( 
						&obj_dom_alias_test::case5, da_test );
					ts_case5->depends_on(ts_init);
					add(ts_case5);

					test_case * ts_case6 = BOOST_CLASS_TEST_CASE( 
						&obj_dom_alias_test::case6, da_test );
					ts_case6->depends_on(ts_init);
					add(ts_case6);

					test_case * ts_case7 = BOOST_CLASS_TEST_CASE( 
						&obj_dom_alias_test::case7, da_test );
					ts_case7->depends_on(ts_init);
					add(ts_case7);
			}

			test_case * ts_case8 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case8, test );
			ts_case8->depends_on(ts_init);
			add(ts_case8);

			// uc_test
			{
					test_case * ts_case9 = BOOST_CLASS_TEST_CASE( 
						&obj_user_conf_test::case9, uc_test );
					ts_case9->depends_on(ts_init);
					add(ts_case9);

					test_case * ts_case10 = BOOST_CLASS_TEST_CASE( 
						&obj_user_conf_test::case10, uc_test );
					ts_case10->depends_on(ts_init);
					add(ts_case10);

					test_case * ts_case11 = BOOST_CLASS_TEST_CASE( 
						&obj_user_conf_test::case11, uc_test );
					ts_case11->depends_on(ts_init);
					add(ts_case11);

					test_case * ts_case12 = BOOST_CLASS_TEST_CASE( 
						&obj_user_conf_test::case12, uc_test );
					ts_case12->depends_on(ts_init);
					add(ts_case12);

					test_case * ts_case13 = BOOST_CLASS_TEST_CASE( 
						&obj_user_conf_test::case13, uc_test );
					ts_case13->depends_on(ts_init);
					add(ts_case13);
			}

			// dom_ls_test
			{
					test_case * ts_case1 = BOOST_CLASS_TEST_CASE( 
						&obj_dom_ls_test::case1, dom_ls_test );
					ts_case1->depends_on(ts_init);
					add(ts_case1);
			}
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
	register_exception_translator<CORBA::Exception>( &et_CORBA_Exception );
	register_exception_translator<CORBA::SystemException>( &et_CORBA_SystemException );

	test->add(new auth_test_suite(ac, av));

    return test.release();
}
 
