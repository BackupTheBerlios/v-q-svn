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

using boost::unit_test_framework::test_suite;
using boost::unit_test_framework::test_case;

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

		void user_add() {
				vq::iauth::auth_info ai;
				ai.user = static_cast<const char *>("test");
				ai.dom = static_cast<const char *>("test");
				ai.pass = static_cast<const char *>("test");
				ai.dir = static_cast<const char *>("dir");
								
				auth->user_add(ai);
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
			test_case * ts_user_add = BOOST_CLASS_TEST_CASE( 
				&auth_test::user_add, test );
			ts_user_add->depends_on(ts_init);

			add(ts_init);
			add(ts_user_add);
		}
};

/**
 *
 */
test_suite* init_unit_test_suite( int ac, char* av[] ) {
	std::auto_ptr<test_suite> test(BOOST_TEST_SUITE("pgsqlauthd tests"));

	test->add(new auth_test_suite(ac, av));

    return test.release();
}
 
