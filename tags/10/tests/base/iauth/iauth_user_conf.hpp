#ifndef __IAUTH_USER_CONF_HPP
#define __IAUTH_USER_CONF_HPP

#include "../../../base/auth.hpp"

#include <getlines.hpp>
#include <split.hpp>

#include <p_strings.h>

#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <vector>
#include <map>

template <class OBJ_TYPE, class WRAPPER>
struct user_conf_test {
		vq::ivq::error_var err;

		OBJ_TYPE & auth;

		WRAPPER wrap;

		user_conf_test( OBJ_TYPE & a ) : auth(a), wrap(a) {
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
			::vq::ivq::id_type dom_id = ::vq::ivq::id_type();
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
					::vq::ivq::user_info ai;
					ui.type = ::vq::ivq::uc_redir;
					typedef std::vector< ::vq::ivq::id_type > id_array;
					id_array ids;
					ids.reserve(users.size()*pfixs.size());
					for( ubeg=users.begin(), uend=users.end(); 
								ubeg!=uend; ++ubeg) {

							ai.id_domain = dom_id;
							ai.pass = boost::lexical_cast<std::string>(dom_id).c_str();
							ai.flags = 0;
							ai.login = ubeg->c_str(); 
							IVQ_ERROR_EQUAL(auth->user_add(ai, 0),
								::vq::ivq::err_no);

							for( pbeg=pfixs.begin(), pend=pfixs.end();
										pbeg!=pend; ++pbeg ) {

									ui.val = pbeg->c_str();
									ui.id_conf = ::vq::ivq::id_type();
									IVQ_ERROR_EQUAL(auth->user_conf_add(dom_id,
										ubeg->c_str(), pbeg->c_str(), ui),
										::vq::ivq::err_no);
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

									IVQ_ERROR_EQUAL(wrap.user_conf_rm(dom_id,
										ubeg->c_str(), pbeg->c_str(),
										ui.id_conf), ::vq::ivq::err_no );
							}
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
					::vq::ivq::id_type dom_id = ::vq::ivq::id_type();
					test_dom_user_add(dom, dom_id);

					string_array conf;
					std::ifstream ifs("data/case10/conf");
					BOOST_REQUIRE(sys::getlines<std::string>(ifs, conf));
					ifs.close();

					::vq::ivq::user_info ai;
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
							
							err = auth->user_add(ai, 0);
							if( err->ec != ::vq::ivq::err_no 
								&& err->ec != ::vq::ivq::err_exists ) {
									BOOST_ERROR(error2str(err));	
							}

							++fc;
							if( fc == fe ) continue;
							
							for( pfix=fc, ++fc; fc!=fe; ++fc )  {
									uia = text::split(*fc, ",");
									BOOST_REQUIRE(uia.size() == 2);
									ui.type = boost::lexical_cast<CORBA::UShort>
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
									ui.type = boost::lexical_cast<CORBA::UShort>
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
			uci.id_conf = static_cast< ::vq::ivq::id_type >(123123123);
			uci.type = 12;
			uci.val = static_cast<const char *>("asd");
			IVQ_ERROR_EQUAL(wrap.user_conf_rep(
				static_cast< ::vq::ivq::id_type >(12),
				static_cast<const char *>("asd"), 
				static_cast<const char *>("cvxcvxcv"),
				uci), ::vq::ivq::err_noent);
		}

		/**
		 * Add domain and user. Add configuration
		 *
		 */
		void case12() {
			const char * dom = "case12.pl";
			std_try {
					::vq::ivq::id_type dom_id = ::vq::ivq::id_type();
					test_dom_user_add(dom, dom_id);

					::vq::ivq::user_info ai;
					ai.id_domain = dom_id;
					ai.pass = static_cast<const char *>("asdasd");
					ai.flags = 0;
					ai.login = static_cast<const char *>("test");
					IVQ_ERROR_EQUAL(auth->user_add(ai, 0), ::vq::ivq::err_no);

					typedef std::vector< ::vq::ivq::user_conf_info > uci_array; 
					uci_array ucis;
					::vq::ivq::user_conf_info uci;
					uci.type = 1;
					uci.val = static_cast<const char *>("1");
					ucis.push_back(uci);
					++uci.type;
					ucis.push_back(uci);
					ucis.push_back(uci);
					uci.type = ::vq::ivq::uc_external;
					ucis.push_back(uci);
					ucis.push_back(uci);
					uci_array::iterator ub, ue;
					
					for( ub=ucis.begin(), ue=ucis.end(); ub!=ue; ++ub ) {
							IVQ_ERROR_EQUAL(auth->user_conf_add(dom_id,
								ai.login, static_cast<const char *>(""),
								*ub ), ::vq::ivq::err_no );
					}
					for( ub=ucis.begin(), ue=ucis.end(); ub!=ue; ++ub ) {
							ub->val = static_cast<const char *>("2");
							IVQ_ERROR_EQUAL(wrap.user_conf_rep(dom_id,
								ai.login, static_cast<const char *>(""),
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
				static_cast< ::vq::ivq::id_type >(123123123), 
				static_cast<const char *>("login"),
				static_cast<const char *>(""),
				12), ::vq::ivq::err_no);
			const char * dom = "case13.pl";
			std_try {
					::vq::ivq::id_type dom_id = ::vq::ivq::id_type();
					test_dom_user_add(dom, dom_id);

					IVQ_ERROR_EQUAL(auth->user_conf_rm_by_type(dom_id,
						static_cast<const char *>("login"),
						static_cast<const char *>(""),
						12), ::vq::ivq::err_no);

					::vq::ivq::user_info ai;
					ai.id_domain = dom_id;
					ai.pass = static_cast<const char *>("asdasd");
					ai.flags = 0;
					ai.login = static_cast<const char *>("test");
					IVQ_ERROR_EQUAL(auth->user_add(ai, 0), ::vq::ivq::err_no);

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

		/**
		 * Try to add entry for user in missing domain
		 */
		void case14() {
			::vq::ivq::user_conf_info uci;
			uci.type = 12;
			uci.val = static_cast<const char *>("asd");

			IVQ_ERROR_EQUAL(auth->user_conf_add(
				static_cast< ::vq::ivq::id_type >(-1),
				static_cast<const char *>("asd"), 
				static_cast<const char *>("cvxcvxcv"),
				uci), ::vq::ivq::err_noent);
		}

}; // struct user_conf_test

#endif // ifndef __IAUTH_USER_CONF_HPP
