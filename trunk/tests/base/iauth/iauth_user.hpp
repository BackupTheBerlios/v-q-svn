#ifndef __IAUTH_USER_HPP
#define __IAUTH_USER_HPP

#include "../../../base/auth.hpp"

#include <getlines.hpp>
#include <split.hpp>

#include <p_strings.h>

#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <vector>
#include <map>

template <class OBJ_TYPE>
struct user_test {
		vq::ivq::error_var err;

		OBJ_TYPE & auth;

		user_test( OBJ_TYPE & a ) : auth(a) {
		}

		/**
		 *
		 */
		void test_dom_rm(const char * dom) {
			::vq::ivq::id_type dom_id = ::vq::ivq::id_type();
			err = auth->dom_id(dom, dom_id);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no );
			err = auth->dom_rm(dom_id);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
		}

		/**
		 *
		 */
		void test_dom_user_add( const char * dom, ::vq::ivq::id_type & dom_id ) {
			err = auth->dom_add(dom, dom_id);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
		
			vq::iauth::user_info ai;
			ai.id_domain = dom_id;
			ai.pass = CORBA::string_dup("pass");
			ai.login = CORBA::string_dup(dom);
			ai.flags = 0;
		
			err = auth->user_add(ai, 0);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
		}

		/**
		 * Add domain. Add user. Check count of elements. Add second domain. Check count of elements for both domains.
		 */
		void case9() {
			const char * dom = "user-test-case9.pl";
			const char * dom1 = "1user-test-case9.pl";
			::vq::ivq::id_type dom_id = ::vq::ivq::id_type();
			::vq::ivq::id_type dom_id1 = ::vq::ivq::id_type();
			std_try {
					test_dom_user_add(dom, dom_id);
					err = auth->dom_add(dom1, dom_id1);
					BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
	
					::vq::ivq::size_type cnt, cnt1;

					IVQ_ERROR_EQUAL(auth->user_cnt_by_dom(dom_id, cnt), ::vq::ivq::err_no);
					BOOST_CHECK_EQUAL( cnt, 1U );
					IVQ_ERROR_EQUAL(auth->user_cnt_by_dom(dom_id1, cnt1), ::vq::ivq::err_no);
					BOOST_CHECK_EQUAL( cnt1, 0U );

					::vq::ivq::user_info_list_var uis;

					IVQ_ERROR_EQUAL(auth->user_ls_by_dom(
						dom_id1, uis), ::vq::ivq::err_no);
					BOOST_CHECK_EQUAL( uis->length(), 0U );

					IVQ_ERROR_EQUAL(auth->user_ls_by_dom(dom_id, uis), ::vq::ivq::err_no);
					BOOST_CHECK_EQUAL( uis->length(), 1U );
					if( uis->length() > 0 ) {
						BOOST_CHECK_EQUAL( uis[0].id_domain, dom_id );
						BOOST_CHECK( ! strcmp(uis[0].pass, "pass") );
						BOOST_CHECK( ! strcmp(uis[0].login, dom) );
						BOOST_CHECK_EQUAL( uis[0].flags, 0 );
					}
			} std_catch
			test_dom_rm(dom);
			test_dom_rm(dom1);
		}

		/**
		 * Try to get users and count of users for non-existing domain
		 */
		void case11() {
			::vq::ivq::size_type cnt;
			::vq::ivq::user_info_list_var uis;
			IVQ_ERROR_EQUAL(auth->user_cnt_by_dom(
				static_cast< ::vq::ivq::id_type >(12342342), cnt), ::vq::ivq::err_no);
			BOOST_CHECK_EQUAL( cnt, 0U );
			IVQ_ERROR_EQUAL(auth->user_ls_by_dom(
				static_cast< ::vq::ivq::id_type >(12342342), uis), ::vq::ivq::err_no);
			BOOST_CHECK_EQUAL( uis->length(), 0U );
		}
}; // struct user_test

#endif // ifndef __IAUTH_USER_CONF_HPP
