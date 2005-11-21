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
#include "../kill_myself.hpp"
#include "../iauth/iauth_common.hpp"

#include <vq.hpp>
#include <text.hpp>
#include <pfstream.hpp>

#include <import_export.h>

#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <memory>
#include <sstream>
#include <map>

using namespace boost::unit_test_framework;

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

			BOOST_REQUIRE(ac>1);

			CORBA::Object_var obj;
			try {
					obj = corbautil::importObjRef(orb, av[1]);
			} catch ( corbautil::ImportExportException & e ) {
					BOOST_ERROR(e.msg);
			}

			vq = vq::ivq::_narrow(obj);

			BOOST_REQUIRE( !CORBA::is_nil(vq) );
		}

		static const char * dom;
		static const char * user;

		::vq::ivq::id_type dom_id;
		/**
		 *
		 */
		void case1() {
				err = vq->dom_id(this->dom, this->dom_id);
				if( ::vq::ivq::err_noent == err->ec ) {
						IVQ_ERROR_EQUAL(
							err=vq->dom_add(this->dom, this->dom_id), 
							::vq::ivq::err_no);
						BOOST_REQUIRE(::vq::ivq::err_no == err->ec);
				}
				err = vq->user_ex(this->dom_id, this->user);
				if( ::vq::ivq::err_noent == err->ec ) {
						::vq::ivq::user_info ai;
						ai.login = this->user;
						ai.pass = this->user;
						ai.flags = 0;
						ai.id_domain = this->dom_id;
						IVQ_ERROR_EQUAL(
							err=vq->user_add(ai, 0), ::vq::ivq::err_no );
						BOOST_REQUIRE(::vq::ivq::err_no == err->ec );
				}
		}

		/**
		 * Check if path to Maildir is relative
		 */
		void case2() {
				std::string path("./domains/"
					+text::split_id(
						boost::lexical_cast<std::string>(this->dom_id), 1)
					+'/'+boost::lexical_cast<std::string>(this->dom_id)
					+'/'+text::split_user(this->user, 3)
					+"/.qmail-"+this->user);
				posix::ipfstream in(path.c_str());
				if(! in) BOOST_ERROR(path);
				BOOST_REQUIRE(in.good());
				std::string ln;
				BOOST_REQUIRE(std::getline(in, ln));
				BOOST_CHECK_EQUAL(ln, (std::string)"./"+this->user+"/Maildir/");
		}

		/**
		 * 
		 */
		void case3() {
			using namespace boost::filesystem;
			using namespace boost::posix_time;

			string login(to_iso_string(ptime(microsec_clock::local_time())));
			string home("./tmp/"+login);
			path phome(home, native);
			::vq::ivq::user_info ai;
			ai.login = login.c_str();
			ai.pass = this->user;
			ai.dir = home.c_str();
			ai.flags = 0;
			ai.id_domain = this->dom_id;

			err = vq->user_rm(this->dom_id, ai.login);
			BOOST_CHECK( ::vq::ivq::err_no == err->ec 
				|| ::vq::ivq::err_noent == err->ec );
			IVQ_ERROR_EQUAL(
				err=vq->user_add(ai, 0), ::vq::ivq::err_no );
			BOOST_REQUIRE(::vq::ivq::err_no == err->ec );

			BOOST_CHECK(exists(phome));
			BOOST_CHECK(exists(phome / "Maildir"));
			BOOST_CHECK(exists(phome / "Maildir" / "new"));
			BOOST_CHECK(exists(phome)
				&& is_directory(phome));
			IVQ_ERROR_EQUAL(vq->user_rm(this->dom_id, ai.login), ::vq::ivq::err_no );
			BOOST_CHECK(! exists(phome));
		}
		
		/**
		 *
		 */
		void cleanup() {
				::vq::ivq::id_type dom_id;
				IVQ_ERROR_EQUAL(vq->dom_id(this->dom, dom_id), ::vq::ivq::err_no);
				IVQ_ERROR_EQUAL(vq->dom_rm(dom_id), ::vq::ivq::err_no);
		}
};

const char * vq_test::dom = "user-conf.pl";
const char * vq_test::user = "user-conf";

/**
 * 
 */
struct vq_test_suite : test_suite {
		struct obj_wrap {
				::vq::ivq_var & obj;
				
				obj_wrap( ::vq::ivq_var & a ) : obj(a) {}
				
				::vq::ivq::error * user_conf_rm( ::vq::ivq::id_type dom_id,
						const char *login, const char *pfix, 
						::vq::ivq::id_type id ) {
					return obj->user_conf_rm(dom_id, login, pfix, id);
				}

				::vq::ivq::error * user_conf_rep( ::vq::ivq::id_type dom_id, 
						const char * login, const char * pfix, 
						const ::vq::ivq::user_conf_info & ui ) {
					return obj->user_conf_rep(dom_id, login, pfix, ui);
				}
		};

		boost::shared_ptr<vq_test> test;

		vq_test_suite(int ac, char *av[]) 
				: test_suite("qmailvqd tests"),
				test(new vq_test(ac, av)) {

			test_case * ts_init = BOOST_CLASS_TEST_CASE( 
				&vq_test::init, test );
			add(ts_init);

			test_case * ts_uc1 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case1, test );
			ts_uc1->depends_on(ts_init);
			add(ts_uc1);

			test_case * ts_uc2 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case2, test );
			ts_uc2->depends_on(ts_init);
			add(ts_uc2);

			test_case * ts_uc3 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case3, test );
			ts_uc3->depends_on(ts_init);
			add(ts_uc3);

			test_case * ts_cleanup = BOOST_CLASS_TEST_CASE( 
				&vq_test::cleanup, test );
			ts_cleanup->depends_on(ts_init);
			add(ts_cleanup);
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
	register_exception_translator<CORBA::Exception>( &et_CORBA_Exception );
	register_exception_translator<CORBA::SystemException>( &et_CORBA_SystemException );

	test->add(new vq_test_suite(ac, av));
	test->add(new kill_myself_suite());

    return test.release();
}
 
