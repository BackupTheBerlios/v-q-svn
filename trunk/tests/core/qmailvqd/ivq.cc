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
#include "../iauth/iauth_user_conf.hpp"
#include "../../../core/vq.hpp"

#include <text.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>

#include <coss/CosNaming.h>

#include <memory>
#include <sstream>
#include <map>

using namespace boost::unit_test_framework;

/**
 * 
 */
void et_vq_null_error( const vq::null_error &e ) {
	BOOST_ERROR("null_error");
}

/**
 * 
 */
void et_vq_except( const vq::except &e ) {
	BOOST_ERROR(std::string("ivq::except: ")+static_cast<const char *>(e.what));
}

/**
 * 
 */
void et_vq_db_error( const vq::db_error &e ) {
	BOOST_ERROR(std::string("ivq::db_error: ")+static_cast<const char *>(e.what));
}

/**
 * 
 */
void et_corba_exception( const CORBA::Exception & e ) {
	std::ostringstream os;
	e._print(os);
	BOOST_ERROR("corba exception: "+os.str());
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
struct vq_test {
		vq::ivq_var vq;
		vq::ivq::error_var err;
		int ac;
		char **av;
		
		vq_test( int ac, char **av ) : ac(ac), av(av) {
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
			name[0].id = CORBA::string_dup ("vq::ivq");
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

			vq = vq::ivq::_narrow(obj);

			BOOST_REQUIRE( !CORBA::is_nil(vq) );
		}

		static const char * uc_dom;
		static const char * uc_user;

		CORBA::String_var uc_dom_id;
		/**
		 *
		 */
		void uc_case1() {
				err = vq->dom_id(this->uc_dom, this->uc_dom_id);
				if( ::vq::ivq::err_noent == err->ec ) {
						IVQ_ERROR_EQUAL(
							err=vq->dom_add(this->uc_dom, this->uc_dom_id), 
							::vq::ivq::err_no);
						BOOST_REQUIRE(::vq::ivq::err_no == err->ec);
				}
				err = vq->user_ex(this->uc_dom_id, this->uc_user);
				if( ::vq::ivq::err_noent == err->ec ) {
						::vq::ivq::auth_info ai;
						ai.login = this->uc_user;
						ai.pass = this->uc_user;
						ai.flags = 0;
						ai.id_domain = this->uc_dom_id;
						IVQ_ERROR_EQUAL(
							err=vq->user_add(ai, FALSE), ::vq::ivq::err_no );
						BOOST_REQUIRE(::vq::ivq::err_no == err->ec );
				}
		}

		/**
		 *
		 */
		void uc_cleanup() {
				CORBA::String_var dom_id;
				IVQ_ERROR_EQUAL(vq->dom_id(this->uc_dom, dom_id), ::vq::ivq::err_no);
				IVQ_ERROR_EQUAL(vq->dom_rm(dom_id), ::vq::ivq::err_no);
		}
};

const char * vq_test::uc_dom = "user-conf.pl";
const char * vq_test::uc_user = "user-conf";

/**
 * 
 */
struct vq_test_suite : test_suite {
		struct obj_wrap {
				::vq::ivq_var & obj;
				
				obj_wrap( ::vq::ivq_var & a ) : obj(a) {}
				
				::vq::ivq::error * user_conf_rm(const char *dom_id,
						const char *login, const char *pfix, const char * id ) {
					return obj->user_conf_rm(dom_id, login, pfix, id);
				}

				::vq::ivq::error * user_conf_rep( const char *dom_id, 
						const char * login, const char * pfix, 
						const ::vq::ivq::user_conf_info & ui ) {
					return obj->user_conf_rep(dom_id, login, pfix, ui);
				}
		};

		typedef user_conf_test<vq::ivq_var, obj_wrap> obj_user_conf_test;

		boost::shared_ptr<vq_test> test;
		boost::shared_ptr< obj_user_conf_test > uc_test;

		vq_test_suite(int ac, char *av[]) 
				: test_suite("qmailvqd tests"),
				test(new vq_test(ac, av)),
				uc_test(new obj_user_conf_test(test->vq)) {

			test_case * ts_init = BOOST_CLASS_TEST_CASE( 
				&vq_test::init, test );
			add(ts_init);

			test_case * ts_uc1 = BOOST_CLASS_TEST_CASE( 
				&vq_test::uc_case1, test );
			ts_uc1->depends_on(ts_init);
			add(ts_uc1);

			test_case * ts_uc_cleanup = BOOST_CLASS_TEST_CASE( 
				&vq_test::uc_cleanup, test );
			ts_uc_cleanup->depends_on(ts_init);
			add(ts_uc_cleanup);
		}
};

/**
 *
 */
test_suite* init_unit_test_suite( int ac, char* av[] ) {
	std::auto_ptr<test_suite> test(BOOST_TEST_SUITE("qmailvqd tests"));

	register_exception_translator<vq::null_error>( &et_vq_null_error );
	register_exception_translator<vq::db_error>( &et_vq_db_error );
	register_exception_translator<vq::except>( &et_vq_except );
	register_exception_translator<CORBA::Exception>( &et_corba_exception );

	test->add(new vq_test_suite(ac, av));

    return test.release();
}
 
