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
#include "../iauth/iauth_common.hpp"
#include "../../../core/logger.hpp"

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
struct logger_test {
		typedef std::deque<std::string> string_array;

		vq::ilogger_var obj;
		vq::ivq::error_var err;
		int ac;
		char **av;
		
		logger_test( int ac, char **av ) : ac(ac), av(av) {
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
			name[0].id = CORBA::string_dup ("vq::ilogger");
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

			this->obj = vq::ilogger::_narrow(obj);

			BOOST_REQUIRE( !CORBA::is_nil(this->obj) );
		}

		/**
		 *
		 */
		void case1() {
			std::ifstream ifs("data/case1/ips");
			string_array ips;
			BOOST_REQUIRE(sys::getlines<std::string>(ifs, ips));
			ifs.close();
			ifs.clear();

			string_array::const_iterator ibeg, iend;
			for( ibeg=ips.begin(), iend=ips.end(); ibeg!=iend; ++ibeg ) {
					BOOST_CHECK_NO_THROW(obj->ip_set(ibeg->c_str()));
			}

			ips.clear();
			ifs.open("data/case1/services");
			BOOST_REQUIRE(sys::getlines<std::string>(ifs, ips));
			ifs.close();
			ifs.clear();
			::vq::ilogger::service_type ser;
			for( ibeg=ips.begin(), iend=ips.end(); ibeg!=iend; ++ibeg ) {
					BOOST_CHECK_NO_THROW(
						ser = lexical_cast< ::vq::ilogger::service_type >(*ibeg) );
					BOOST_CHECK_NO_THROW(obj->service_set(ser));
			}

			ips.clear();
			ifs.open("data/case1/domains");
			BOOST_REQUIRE(sys::getlines<std::string>(ifs, ips));
			ifs.close();
			ifs.clear();
			for( ibeg=ips.begin(), iend=ips.end(); ibeg!=iend; ++ibeg ) {
					BOOST_CHECK_NO_THROW(obj->domain_set(ibeg->c_str()));
			}

			ips.clear();
			ifs.open("data/case1/logins");
			BOOST_REQUIRE(sys::getlines<std::string>(ifs, ips));
			ifs.close();
			ifs.clear();
			for( ibeg=ips.begin(), iend=ips.end(); ibeg!=iend; ++ibeg ) {
					BOOST_CHECK_NO_THROW(obj->domain_set(ibeg->c_str()));
			}
		}
	
}; // struct logger_test

/**
 * 
 */
struct logger_test_suite : test_suite {

		boost::shared_ptr<logger_test> test;

		logger_test_suite(int ac, char *av[]) 
				: test_suite("pgsqllogd tests"), 
				test(new logger_test(ac, av)) {

			test_case * ts_init = BOOST_CLASS_TEST_CASE( 
				&logger_test::init, test );
			add(ts_init);

			test_case * ts_case1 = BOOST_CLASS_TEST_CASE(
				&logger_test::case1, test );
			ts_case1->depends_on(ts_init);
			add(ts_case1);
		}
};

/**
 *
 */
test_suite* init_unit_test_suite( int ac, char* av[] ) {
	std::auto_ptr<test_suite> test(BOOST_TEST_SUITE("pgsqllogd tests"));

	register_exception_translator<vq::null_error>( &et_vq_null_error );
	register_exception_translator<vq::db_error>( &et_vq_db_error );
	register_exception_translator<vq::except>( &et_vq_except );
	register_exception_translator<CORBA::Exception>( &et_corba_exception );

	test->add(new logger_test_suite(ac, av));

    return test.release();
}
 
