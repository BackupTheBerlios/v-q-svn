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

#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>

#include <coss/CosNaming.h>

#include <memory>
#include <sstream>
#include <map>

using namespace boost::unit_test_framework;

#define std_try { try
#define std_catch catch( vq::null_error & e ) { \
	BOOST_ERROR("null_error"); \
} catch( vq::db_error & e ) { \
	BOOST_ERROR(e.what); \
} catch( vq::except & e ) { \
	BOOST_ERROR(e.what); \
} }
	
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
struct auth_test {
		vq::iauth_var auth;
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
		 * Add domain, 
		 * add users, 
		 * remove uses, 
		 * remove domain.
		 */
		void case1() {
			const char * dom = "case1.pl";
			const char *users[] = { "s", "asdasd", "vxcvcxvxcvxvcv" };
			unsigned users_cnt= sizeof(users)/sizeof(*users);
			
			CORBA::String_var dom_id;
			BOOST_CHECK_EQUAL(auth->dom_add(dom, dom_id), vq::iauth::err_no);

			vq::iauth::auth_info ai;
			ai.id_domain = CORBA::string_dup(dom_id);
			ai.pass = CORBA::string_dup("pass");
			ai.dir = CORBA::string_dup("dir");

			for( unsigned i=0; i<users_cnt; ++i ) {
					ai.login = CORBA::string_dup(users[i]);
								
					BOOST_CHECK_EQUAL(auth->user_add(ai, FALSE), 
						vq::iauth::err_no);
					BOOST_CHECK(*ai.id_user);
					BOOST_CHECK(atoi(ai.id_user)>0);
			}

			CORBA::String_var id_user;
			for( unsigned i=0; i<users_cnt; ++i ) {
					BOOST_CHECK_EQUAL(auth->user_id(dom_id, users[i], id_user),
						vq::iauth::err_no);
					BOOST_CHECK_EQUAL(auth->user_rm(dom_id, id_user),
						vq::iauth::err_no);
			}

			BOOST_CHECK_EQUAL(auth->dom_rm(dom_id), vq::iauth::err_no);
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
			BOOST_CHECK_EQUAL(auth->dom_add(dom, dom_id), vq::iauth::err_no);
			BOOST_CHECK_EQUAL(auth->dom_add(dom, dom_id1), vq::iauth::err_exists);

			// 2.
			CORBA::String_var dom_id2;
			BOOST_CHECK_EQUAL(auth->dom_id(dom, dom_id2), vq::iauth::err_no);
			BOOST_CHECK( *dom_id2 && atoi(dom_id2) > 0);

			// 3.
			CORBA::String_var dom_id3;
			BOOST_CHECK_EQUAL(auth->dom_id(dom_mixed, dom_id3), vq::iauth::err_no);
			BOOST_CHECK( !strcmp(dom_id2, dom_id3) );

			// 4.
			BOOST_CHECK_EQUAL(auth->dom_rm(dom_id), vq::iauth::err_no);
			BOOST_CHECK_EQUAL(auth->dom_rm(dom_id), vq::iauth::err_no);
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
						BOOST_CHECK_EQUAL(auth->dom_id(doms[doms_cnt], id),
							vq::iauth::err_noent);
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
			
					BOOST_CHECK_EQUAL(auth->dom_add(dom, dom_id),
						vq::iauth::err_no);

					vq::iauth::auth_info ai;
					ai.id_domain = CORBA::string_dup(dom_id);
					ai.pass = CORBA::string_dup("pass");
					ai.dir = CORBA::string_dup("dir");

					for( unsigned i=0; i<users_cnt; ++i ) {
							ai.login = CORBA::string_dup(users[i]);
								
							BOOST_CHECK_EQUAL(auth->user_add(ai, FALSE), 
								vq::iauth::err_no);
							BOOST_CHECK(*ai.id_user);
							BOOST_CHECK(atoi(ai.id_user)>0);
					}
			} std_catch

			BOOST_CHECK_EQUAL(auth->dom_id(dom, dom_id), vq::iauth::err_no);
			BOOST_CHECK_EQUAL(auth->dom_rm(dom_id), vq::iauth::err_no);
		}

		/**
		 * Get all banned emails, remove all of them
		 * 
		 */
		void case5() {
			CORBA::String_var dom_id;

			std_try {
					vq::iauth::email_banned_list_var ebs;
					BOOST_CHECK_EQUAL(auth->eb_ls(ebs), vq::iauth::err_no);
					CORBA::ULong s = ebs->length();
					for( CORBA::ULong i=0; i<s; ++i ) {
							BOOST_CHECK_EQUAL(auth->eb_rm(ebs[i].re_domain,
								ebs[i].re_login), 
								vq::iauth::err_no);
					}
					BOOST_CHECK_EQUAL(auth->dom_add("case5.pl", dom_id), 
						vq::iauth::err_no);
			
					vq::iauth::email_banned eb;
					BOOST_CHECK_EQUAL(auth->eb_add(
						static_cast<const char *>(".*"),
						static_cast<const char *>("root")), vq::iauth::err_no);
					BOOST_CHECK_EQUAL(auth->eb_add(
						static_cast<const char *>("case5"),
						static_cast<const char *>(".*")), vq::iauth::err_no);

					BOOST_CHECK_EQUAL(auth->eb_ls(ebs), vq::iauth::err_no);
					BOOST_CHECK_EQUAL(ebs->length(), 2U);
					if( !strcmp(ebs[0].re_domain, ".*") ) {
							BOOST_CHECK(!strcmp(ebs[1].re_domain, "case5"));
							BOOST_CHECK(!strcmp(ebs[1].re_login, ".*"));
							BOOST_CHECK(!strcmp(ebs[0].re_login, "root"));
					} else {
							BOOST_CHECK(!strcmp(ebs[0].re_domain, "case5"));
							BOOST_CHECK(!strcmp(ebs[0].re_login, ".*"));
							BOOST_CHECK(!strcmp(ebs[1].re_domain, ".*"));
							BOOST_CHECK(!strcmp(ebs[1].re_login, "root"));
					}

					vq::iauth::auth_info ai;
					ai.id_domain = CORBA::string_dup(dom_id);
					ai.pass = CORBA::string_dup("pass");
					ai.dir = CORBA::string_dup("dir");
					ai.login = CORBA::string_dup("aroote");
					ai.flags = 0;
			
					BOOST_CHECK_EQUAL(auth->user_add(ai, TRUE), 
						vq::iauth::err_user_inv);
					BOOST_CHECK_EQUAL(auth->user_add(ai, TRUE), 
						vq::iauth::err_user_inv);

					BOOST_CHECK_EQUAL(auth->user_add(ai, FALSE), 
						vq::iauth::err_no);
			} std_catch

			BOOST_CHECK_EQUAL(auth->dom_id("case5.pl", dom_id), vq::iauth::err_no );
			BOOST_CHECK_EQUAL(auth->dom_rm(dom_id), vq::iauth::err_no);
		}

		/**
		 * Add domain and user if they don't exists
		 * Change password for user, test if it is changed
		 */
		void case6() {
			const char * dom = "case6.pl";
			CORBA::String_var dom_id;
			if( auth->dom_id(dom, dom_id) != vq::iauth::err_no ) {
					BOOST_CHECK_EQUAL(auth->dom_add(dom, dom_id), 
						vq::iauth::err_no);
			}

			vq::iauth::auth_info ai;
			ai.id_domain = CORBA::string_dup(dom_id);
			ai.pass = CORBA::string_dup("pass");
			ai.dir = CORBA::string_dup("dir");
			ai.login = CORBA::string_dup(dom);
			ai.flags = 0;

			if( auth->user_id(dom_id, dom, ai.id_user) != vq::iauth::err_no ) {
					BOOST_CHECK_EQUAL(auth->user_add(ai, FALSE), 
						vq::iauth::err_no);
			}
			std::string now(boost::posix_time::to_iso_string(
				boost::posix_time::second_clock::local_time()));
			BOOST_CHECK_EQUAL(auth->user_pass(dom_id, ai.id_user, now.c_str()),
				vq::iauth::err_no );

			vq::iauth::auth_info aicur;
			aicur.id_domain = ai.id_domain;
			aicur.id_user = ai.id_user;
			aicur.pass = now.c_str();
			BOOST_CHECK_EQUAL(auth->user_auth(aicur), vq::iauth::err_no );
			BOOST_CHECK_EQUAL(aicur.flags, 0);

			aicur.pass = CORBA::string_dup("");
			BOOST_CHECK_EQUAL(auth->user_auth(aicur), vq::iauth::err_pass_inv);
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
			BOOST_CHECK_EQUAL(auth->dom_id(dom, dom_id), 
				vq::iauth::err_no);
			BOOST_REQUIRE(*dom_id);
			for( unsigned i=0; i<reas_cnt; ++i ) {
					BOOST_CHECK_EQUAL(auth->dra_add(dom_id, reas[i]),
						vq::iauth::err_no );
			}
			vq::iauth::string_list_var from_db, from_db1;
			BOOST_CHECK_EQUAL(auth->dra_ls_by_dom(dom_id, from_db),
				vq::iauth::err_no);
			BOOST_CHECK_EQUAL(from_db->length(), reas_cnt);
			// need to compare all of them
			std::map<std::string, char> in_db;
			for( unsigned i=0, s=from_db->length(); i<s; ++i ) {
					in_db[static_cast<const char *>(from_db[i])] = 1;
			}
			for( unsigned i=0; i<reas_cnt; ++i ) {
					BOOST_CHECK_EQUAL(in_db[reas[i]], 1);
			}
			
			BOOST_CHECK_EQUAL(auth->dra_ls_by_dom(
				static_cast<const char *>("-1"), from_db1), 
				vq::iauth::err_no );
			BOOST_CHECK_EQUAL(from_db1->length(), 0U);

			BOOST_CHECK_EQUAL(auth->dra_rm(dom_id, reas[0]),
				vq::iauth::err_no );
			BOOST_CHECK_EQUAL(auth->dra_ls_by_dom(dom_id, from_db),
				vq::iauth::err_no);
			BOOST_CHECK_EQUAL(from_db->length(), reas_cnt-1);
			
			BOOST_CHECK_EQUAL(auth->dra_rm_by_dom(dom_id), vq::iauth::err_no );
			BOOST_CHECK_EQUAL(auth->dra_ls_by_dom(dom_id, from_db),
				vq::iauth::err_no);
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
					BOOST_CHECK_EQUAL(auth->dom_id(dom, dom_id), 
						vq::iauth::err_no);
					BOOST_REQUIRE(*dom_id);
		
					::vq::iauth::quota_type bytes_max, files_max;
					::vq::iauth::quota_type ubytes, ufiles;
					BOOST_CHECK_EQUAL(auth->qt_user_def_get(dom_id, 
						bytes_max, files_max), ::vq::iauth::err_no );
					BOOST_CHECK_EQUAL(bytes_max, 0U);
					BOOST_CHECK_EQUAL(files_max, 0U);
		
					// changing default quota for users
					bytes_max = 1000;
					files_max = 500;
					BOOST_CHECK_EQUAL(auth->qt_user_def_set(dom_id,
						bytes_max, files_max), ::vq::iauth::err_no);
					BOOST_CHECK_EQUAL(auth->qt_user_def_get(dom_id,
						ubytes, ufiles), ::vq::iauth::err_no );
					BOOST_CHECK_EQUAL(files_max, ufiles);
					BOOST_CHECK_EQUAL(bytes_max, ubytes);
		
					// adding user
					vq::iauth::auth_info ai;
					ai.id_domain = CORBA::string_dup(dom_id);
					ai.pass = CORBA::string_dup("pass");
					ai.dir = CORBA::string_dup("dir");
					ai.login = CORBA::string_dup("case8");
					ai.flags = 0;
		
					BOOST_CHECK_EQUAL(auth->user_add(ai, FALSE), 
						vq::iauth::err_no);
		
					// check quota for created user
					BOOST_CHECK_EQUAL(auth->qt_user_get(dom_id, ai.id_user,
						ubytes, ufiles), ::vq::iauth::err_no );
					BOOST_CHECK_EQUAL(ubytes, bytes_max);
					BOOST_CHECK_EQUAL(ufiles, files_max);
		
					// change quotas for user
					files_max += 501;
					bytes_max += 501;
					BOOST_CHECK_EQUAL(auth->qt_user_set(dom_id, ai.id_user,
						bytes_max, files_max), ::vq::iauth::err_no );
			
					// check quotas
					BOOST_CHECK_EQUAL(auth->qt_user_get(dom_id, ai.id_user,
						ubytes, ufiles), ::vq::iauth::err_no );
					BOOST_CHECK_EQUAL(ubytes, bytes_max);
					BOOST_CHECK_EQUAL(ufiles, files_max);
			} std_catch

			// remove user
			CORBA::String_var user_id;
			BOOST_CHECK_EQUAL(auth->user_id(dom_id, "case8", user_id), 
				::vq::iauth::err_no );
			BOOST_CHECK_EQUAL(auth->user_rm(dom_id, user_id), 
				::vq::iauth::err_no );

			// revert to default values
			BOOST_CHECK_EQUAL(auth->dom_id(dom, dom_id),
				::vq::iauth::err_no );
			BOOST_CHECK_EQUAL(auth->qt_user_def_set(dom_id, 0U, 0U),
				::vq::iauth::err_no );
		}
};

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
			ts_case7->depends_on(ts_case6);
			add(ts_case7);

			test_case * ts_case8 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case8, test );
			ts_case8->depends_on(ts_case6);
			add(ts_case8);

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
 
