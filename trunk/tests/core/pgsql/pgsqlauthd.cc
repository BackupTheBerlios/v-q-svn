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

#include <coss/CosNaming.h>

#include <memory>
#include <sstream>

using namespace boost::unit_test_framework;

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
		 * Add domain, add users, remove uses, remove domain.
		 * No errors.
		 */
		void case1() {
			const char * dom = "test.pl";
			const char * user = "test";
			BOOST_CHECK_EQUAL(auth->dom_add(dom), vq::iauth::err_no);
				
			vq::iauth::auth_info ai;
			ai.user = CORBA::string_dup(user);
			ai.dom = CORBA::string_dup(dom);
			ai.pass = CORBA::string_dup("test");
			ai.dir = CORBA::string_dup("dir");
								
			BOOST_CHECK_EQUAL(auth->user_add(ai), vq::iauth::err_no);

			//BOOST_CHECK_EQUAL(auth->user_exists());
			
			BOOST_CHECK_EQUAL(auth->user_rm(dom, user), vq::iauth::err_no);
			BOOST_CHECK_EQUAL(auth->dom_rm(dom), vq::iauth::err_no);
		}

		/**
		 * Adding domain, adding the same domain again (err_exists),
		 * removing domain, removing again (no errors).
		 */
		void case2() {
			const char *dom = "test.pl";

			BOOST_CHECK_EQUAL(auth->dom_add(dom), vq::iauth::err_no);
			BOOST_CHECK_EQUAL(auth->dom_add(dom), vq::iauth::err_exists);
			BOOST_CHECK_EQUAL(auth->dom_rm(dom), vq::iauth::err_no);
			BOOST_CHECK_EQUAL(auth->dom_rm(dom), vq::iauth::err_no);
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

			/*
			test_case * ts_case1 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case1, test );
			ts_case1->depends_on(ts_init);
			add(ts_case1);
			*/
			test_case * ts_case2 = BOOST_CLASS_TEST_CASE( 
				&auth_test::case2, test );
			ts_case2->depends_on(ts_init);
			add(ts_case2);
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
 
