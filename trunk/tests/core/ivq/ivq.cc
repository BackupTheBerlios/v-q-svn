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
#include "../iauth/iauth_dom_alias.hpp"
#include "../iauth/iauth_dom_ls.hpp"
#include "../../../core/vq.hpp"

#include <text.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/regex.hpp>

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

		/**
		 * Try to get id/name of not existing domain.
		 */
		void case1() {
			const char * dom = "nosuchdomain.pl";
			const char * did = "234234";
			
			CORBA::String_var dom_id;
			::vq::ivq::error_var ev(vq->dom_id(dom, dom_id));
			BOOST_CHECK_EQUAL(ev->ec, ::vq::ivq::err_noent);

			CORBA::String_var dom_name;
			ev = vq->dom_name(did, dom_name);
			BOOST_CHECK_EQUAL(ev->ec, ::vq::ivq::err_noent);
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
			err = vq->dom_add(dom, dom_id);
			IVQ_ERROR_EQUAL(err, vq::ivq::err_no);
			err = vq->dom_add(dom, dom_id1);
			IVQ_ERROR_EQUAL(err, vq::ivq::err_exists);

			// 2.
			CORBA::String_var dom_id2;
			err = vq->dom_id(dom, dom_id2);
			IVQ_ERROR_EQUAL(err, vq::ivq::err_no);
			BOOST_CHECK( *dom_id2 && atoi(dom_id2) > 0);
			dom_id = dom_id2;

			// 3.
			CORBA::String_var dom_id3;
			err = vq->dom_id(dom_mixed, dom_id3);
			IVQ_ERROR_EQUAL(err, vq::ivq::err_no);
			BOOST_CHECK( !strcmp(dom_id2, dom_id3) );

			// 4.
			err = vq->dom_rm(dom_id);
			IVQ_ERROR_EQUAL(err, vq::ivq::err_no);
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
						IVQ_ERROR_EQUAL(vq->dom_id(doms[doms_cnt], id),
							vq::ivq::err_noent);
						BOOST_CHECK(!*id);
				} while(doms_cnt --);
			}
		}

		/**
		 * trying to get name of existing domains
		 */
		void case_dom_name1() {
			const char * doms[] = {
					"teADSFSCdf.dfDFGf.dfdf.fd",
					"cx",
					"xcvxc.xcv.xcvxcvDFGDFGcvADcADSASDASD.xcv.v.cv.sdf.gg.dfg.dfg.fdg",
					"xcv.xcv",
					"xcv.x-c-234234234.989v"
			};
			unsigned doms_cnt = sizeof doms/sizeof *doms;
			typedef std::map<CORBA::String_var, std::string> id_name_map;
			id_name_map id2name;
			CORBA::String_var dom_id, dom_name;

			for( unsigned i=0; i<doms_cnt; ++i ) {
					err = vq->dom_add(doms[i], dom_id);
					IVQ_ERROR_EQUAL(err, vq::ivq::err_no);
					id2name[dom_id] = text::lower(doms[i]);
			}

			for( id_name_map::const_iterator beg=id2name.begin(), end=id2name.end();
						beg!=end; ++beg ) {
					err = vq->dom_name(beg->first, dom_name);
					IVQ_ERROR_EQUAL(err, vq::ivq::err_no);
					BOOST_CHECK_EQUAL(beg->second, 
						static_cast<const char *>(dom_name));
			}

			for( id_name_map::const_iterator beg=id2name.begin(), end=id2name.end();
						beg!=end; ++beg ) {
					err = vq->dom_name(beg->first, dom_name);
					IVQ_ERROR_EQUAL(err, vq::ivq::err_no);
					BOOST_CHECK_EQUAL(beg->second, 
						static_cast<const char *>(dom_name));
					
					err = vq->dom_rm(beg->first);
					IVQ_ERROR_EQUAL(err, vq::ivq::err_no);
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
					const char *users[] = { 
							"s", 
							"asdasd", 
							"vxcvASDxcvxvcv", 
							"xCADSASxzc" 
					};
					unsigned users_cnt= sizeof(users)/sizeof(*users);
			
					IVQ_ERROR_EQUAL(vq->dom_add(dom, dom_id),
						vq::ivq::err_no);

					vq::ivq::user_info ai;
					ai.id_domain = CORBA::string_dup(dom_id);
					ai.pass = CORBA::string_dup("pass");
					ai.dir = CORBA::string_dup("dir");
					ai.flags = ::vq::ivq::uif_pop3_blk;

					for( unsigned i=0; i<users_cnt; ++i ) {
							ai.login = users[i];
								
							IVQ_ERROR_EQUAL(vq->user_add(ai, FALSE), 
								vq::ivq::err_no);
					}
			} std_catch

			IVQ_ERROR_EQUAL(vq->dom_id(dom, dom_id), vq::ivq::err_no);
			IVQ_ERROR_EQUAL(vq->dom_rm(dom_id), vq::ivq::err_no);
		}

		/**
		 * Get all banned emails, remove all of them
		 */
		void case5() {
			CORBA::String_var dom_id;

			std_try {
					::vq::ivq::email_banned_list_var ebs;
					IVQ_ERROR_EQUAL(vq->eb_ls(ebs), vq::ivq::err_no);
					CORBA::ULong s = ebs->length();
					for( CORBA::ULong i=0; i<s; ++i ) {
							IVQ_ERROR_EQUAL(
								vq->eb_rm(ebs[i].re_domain, ebs[i].re_login), 
								vq::ivq::err_no);
					}
					IVQ_ERROR_EQUAL(vq->dom_add("case5.pl", dom_id), 
						vq::ivq::err_no);
			
					IVQ_ERROR_EQUAL(vq->eb_add(
						static_cast<const char *>(".*"),
						static_cast<const char *>("root")), vq::ivq::err_no);
					IVQ_ERROR_EQUAL(vq->eb_add(
						static_cast<const char *>("case5"),
						static_cast<const char *>(".*")), vq::ivq::err_no);

					IVQ_ERROR_EQUAL(vq->eb_ls(ebs), vq::ivq::err_no);
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

					vq::ivq::user_info ai;
					ai.id_domain = CORBA::string_dup(dom_id);
					ai.pass = CORBA::string_dup("pass");
					ai.dir = CORBA::string_dup("dir");
					ai.login = CORBA::string_dup("aroote");
					ai.flags = 0;
			
					IVQ_ERROR_EQUAL(vq->user_add(ai, TRUE), 
						vq::ivq::err_user_inv);
					IVQ_ERROR_EQUAL(vq->user_add(ai, TRUE), 
						vq::ivq::err_user_inv);

					IVQ_ERROR_EQUAL(vq->user_add(ai, FALSE), 
						vq::ivq::err_no);
			} std_catch

			IVQ_ERROR_EQUAL(vq->dom_id("case5.pl", dom_id), vq::ivq::err_no );
			IVQ_ERROR_EQUAL(vq->dom_rm(dom_id), vq::ivq::err_no);
		}

		/**
		 * Add domain and user if they don't exists
		 * Change password for user, test if it is changed
		 */
		void case6() {
			const char * dom = "case6.pl";
			CORBA::String_var dom_id;

			err = vq->dom_id(dom, dom_id);
			if( err->ec != vq::ivq::err_no ) {
					IVQ_ERROR_EQUAL(vq->dom_add(dom, dom_id), 
						vq::ivq::err_no);
			}

			vq::ivq::user_info ai;
			ai.id_domain = CORBA::string_dup(dom_id);
			ai.pass = CORBA::string_dup("pass");
			ai.dir = CORBA::string_dup("dir");
			ai.login = CORBA::string_dup(dom);
			ai.flags = 0;

			err = vq->user_ex(dom_id, ai.login);
			if( err->ec == vq::ivq::err_noent ) {
					IVQ_ERROR_EQUAL(vq->user_add(ai, FALSE), 
						vq::ivq::err_no);
			}
			std::string now(boost::posix_time::to_iso_string(
				boost::posix_time::second_clock::local_time()));
			IVQ_ERROR_EQUAL(vq->user_pass(dom_id, ai.login, now.c_str()),
				vq::ivq::err_no );

			vq::ivq::user_info aicur;
			aicur.id_domain = ai.id_domain;
			aicur.login = ai.login;
			IVQ_ERROR_EQUAL(vq->user_get(aicur), vq::ivq::err_no );
			BOOST_CHECK_EQUAL(aicur.flags, ai.flags);
			BOOST_CHECK_EQUAL(now, static_cast<const char *>(aicur.pass));
			BOOST_CHECK(boost::regex_match(
				static_cast<const char *>(aicur.dir), 
				boost::regex("./domains/[0-9]*//[0-9]*/c/a/s//case6.pl")));
		}
#if 0
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
				vq::ivq::err_no);
			BOOST_REQUIRE(*dom_id);
			for( unsigned i=0; i<reas_cnt; ++i ) {
					BOOST_CHECK_EQUAL(auth->dra_add(dom_id, reas[i]),
						vq::ivq::err_no );
			}
			vq::iauth::string_list_var from_db, from_db1;
			BOOST_CHECK_EQUAL(auth->dra_ls_by_dom(dom_id, from_db),
				vq::ivq::err_no);
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
				vq::ivq::err_no );
			BOOST_CHECK_EQUAL(from_db1->length(), 0U);

			BOOST_CHECK_EQUAL(auth->dra_rm(dom_id, reas[0]),
				vq::ivq::err_no );
			BOOST_CHECK_EQUAL(auth->dra_ls_by_dom(dom_id, from_db),
				vq::ivq::err_no);
			BOOST_CHECK_EQUAL(from_db->length(), reas_cnt-1);
			
			BOOST_CHECK_EQUAL(auth->dra_rm_by_dom(dom_id), vq::ivq::err_no );
			BOOST_CHECK_EQUAL(auth->dra_ls_by_dom(dom_id, from_db),
				vq::ivq::err_no);
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
						vq::ivq::err_no);
					BOOST_REQUIRE(*dom_id);
		
					::vq::iauth::quota_type bytes_max, files_max;
					::vq::iauth::quota_type ubytes, ufiles;
					BOOST_CHECK_EQUAL(auth->qt_user_def_get(dom_id, 
						bytes_max, files_max), ::vq::ivq::err_no );
					BOOST_CHECK_EQUAL(bytes_max, 0U);
					BOOST_CHECK_EQUAL(files_max, 0U);
		
					// changing default quota for users
					bytes_max = 1000;
					files_max = 500;
					BOOST_CHECK_EQUAL(auth->qt_user_def_set(dom_id,
						bytes_max, files_max), ::vq::ivq::err_no);
					BOOST_CHECK_EQUAL(auth->qt_user_def_get(dom_id,
						ubytes, ufiles), ::vq::ivq::err_no );
					BOOST_CHECK_EQUAL(files_max, ufiles);
					BOOST_CHECK_EQUAL(bytes_max, ubytes);
		
					// adding user
					vq::iauth::user_info ai;
					ai.id_domain = CORBA::string_dup(dom_id);
					ai.pass = CORBA::string_dup("pass");
					ai.dir = CORBA::string_dup("dir");
					ai.login = CORBA::string_dup("case8");
					ai.flags = 0;
		
					BOOST_CHECK_EQUAL(auth->user_add(ai, FALSE), 
						vq::ivq::err_no);
		
					// check quota for created user
					BOOST_CHECK_EQUAL(auth->qt_user_get(dom_id, ai.id_user,
						ubytes, ufiles), ::vq::ivq::err_no );
					BOOST_CHECK_EQUAL(ubytes, bytes_max);
					BOOST_CHECK_EQUAL(ufiles, files_max);
		
					// change quotas for user
					files_max += 501;
					bytes_max += 501;
					BOOST_CHECK_EQUAL(auth->qt_user_set(dom_id, ai.id_user,
						bytes_max, files_max), ::vq::ivq::err_no );
			
					// check quotas
					BOOST_CHECK_EQUAL(auth->qt_user_get(dom_id, ai.id_user,
						ubytes, ufiles), ::vq::ivq::err_no );
					BOOST_CHECK_EQUAL(ubytes, bytes_max);
					BOOST_CHECK_EQUAL(ufiles, files_max);
			} std_catch

			// remove user
			CORBA::String_var user_id;
			BOOST_CHECK_EQUAL(auth->user_id(dom_id, "case8", user_id), 
				::vq::ivq::err_no );
			BOOST_CHECK_EQUAL(auth->user_rm(dom_id, user_id), 
				::vq::ivq::err_no );

			// revert to default values
			BOOST_CHECK_EQUAL(auth->dom_id(dom, dom_id),
				::vq::ivq::err_no );
			BOOST_CHECK_EQUAL(auth->qt_user_def_set(dom_id, 0U, 0U),
				::vq::ivq::err_no );
		}
#endif // if 0

		/**
		 * Trying to add domains which have invalid name.
		 * All calls should return err_dom_inv
		 */
		void case9() {
			const char * doms[] = {
					"asd asd", // space
					"azxc-@zxc.zc", // @
					"42534%^&asdasd.c", // %^&
					"   ", // space
					"\"", // "
					"asdasd	asdasdasd", // tabulator
					"()~~~~~~xaxczxv.xcv", // ()~
					"zxcxc.zxczxc.z///", // /
			};
			unsigned doms_cnt = sizeof doms/sizeof *doms;
			CORBA::String_var id;
			if( doms_cnt -- ) {
					do {
							IVQ_ERROR_EQUAL(vq->dom_add(doms[doms_cnt], id),
								vq::ivq::err_dom_inv);
					} while( doms_cnt -- );
			}
		}

		/**
		 * Validating invalid domains' names
		 * All calls should return err_dom_inv
		 */
		void case10() {
			const char * doms[] = {
					"asd asd", // space
					"azxc-@zxc.zc", // @
					"42534%^&asdasd.c", // %^&
					"   ", // space
					"\"", // "
					"asdasd	asdasdasd", // tabulator
					"()~~~~~~xaxczxv.xcv", // ()~
					"zxcxc.zxczxc.z///", // /
			};
			unsigned doms_cnt = sizeof doms/sizeof *doms;
			if( doms_cnt -- ) {
					do {
							IVQ_ERROR_EQUAL(vq->dom_val(doms[doms_cnt]),
								vq::ivq::err_dom_inv);
					} while( doms_cnt -- );
			}
		}

		/**
		 * Checking if users exist (none of them exists)
		 * First we create domains (using doms as names), then
		 * we try to:
		 * - get users from created domains (no users),
		 * - change password for users
		 * - get informations about users
		 * then we remove created domains, then we check for all of them 
		 * again all those actions (no domains and no users)
		 */
		void case11() {
				CORBA::String_var doms[] = {
						"123",
						"234",
						"23445",
						"34"
				};
				int doms_cnt = sizeof doms/sizeof *doms;
				CORBA::String_var users[] = {
						"aaSDAD",
						"z",
						"123324",
						"SDFWERCvvcxvxcvcv",
						"erfdgdfgdfgdfgdfgfdgfdgdfgdfgXCVCBCVXBESFDSG-DFGSFDG"
				};
				int users_cnt = sizeof users/sizeof *users;
				typedef std::vector<CORBA::String_var> dom_ids_array;
				dom_ids_array dom_ids;
				dom_ids.reserve(doms_cnt);

				CORBA::String_var id;
				for( int i=0; i < doms_cnt; ++i ) {
					err = vq->dom_id(doms[i], id);
					if( ::vq::ivq::err_noent == err->ec ) {
						IVQ_ERROR_EQUAL(err=vq->dom_add(doms[i], id), 
								::vq::ivq::err_no);
					}
					if( ::vq::ivq::err_no == err->ec )
							dom_ids.push_back(id);
				}

				::vq::ivq::user_info ai;
				for( dom_ids_array::size_type i=0, s=dom_ids.size(); i<s; ++i ) {
						ai.id_domain = dom_ids[i];
						for( int j=0; j < users_cnt; ++j ) {
								IVQ_ERROR_EQUAL(vq->user_ex(dom_ids[i], 
									users[j]), ::vq::ivq::err_noent);
								IVQ_ERROR_EQUAL(vq->user_pass(dom_ids[i], 
									users[j], static_cast<const char *>("zx")), 
									::vq::ivq::err_noent);
								ai.login = users[j];
								IVQ_ERROR_EQUAL(vq->user_get(ai), 
									::vq::ivq::err_noent);
						}
				}

				for( dom_ids_array::size_type i=0, s=dom_ids.size(); i<s; ++i ) {
					IVQ_ERROR_EQUAL(vq->dom_rm(dom_ids[i]), ::vq::ivq::err_no);
				}

				for( dom_ids_array::size_type i=0, s=dom_ids.size(); i<s; ++i ) {
						ai.id_domain = dom_ids[i];
						for( int j=0; j < users_cnt; ++j ) {
								IVQ_ERROR_EQUAL(vq->user_ex(dom_ids[i], 
										users[j]), ::vq::ivq::err_noent);
								IVQ_ERROR_EQUAL(vq->user_pass(dom_ids[i], 
									users[j], static_cast<const char *>("zx")), 
									::vq::ivq::err_noent);
								ai.login = users[j];
								IVQ_ERROR_EQUAL(vq->user_get(ai), 
									::vq::ivq::err_noent);
						}
				}
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
						::vq::ivq::user_info ai;
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
		 * If user_conf_rep is called and ui.type differs than current
		 * it should return err_func_ni (funtion not implemented).
		 */
		void uc_case2() {
				::vq::ivq::user_conf_info ui;
				ui.type = ::vq::ivq::uc_redir;
				ui.val = static_cast<const char *>("asdasdasd");
				IVQ_ERROR_EQUAL(err=vq->user_conf_add(this->uc_dom_id,
					this->uc_user, static_cast<const char *>(""), ui ), 
					::vq::ivq::err_no );
				BOOST_REQUIRE(::vq::ivq::err_no == err->ec);
				ui.type += ::vq::ivq::uc_external;
				IVQ_ERROR_EQUAL(err=vq->user_conf_rep(this->uc_dom_id,
					this->uc_user, static_cast<const char *>(""), ui ), 
					::vq::ivq::err_func_ni );
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
		typedef dom_alias_test<vq::ivq_var> obj_dom_alias_test;
		typedef dom_ls_test<vq::ivq_var> obj_dom_ls_test;

		boost::shared_ptr<vq_test> test;
		boost::shared_ptr< obj_user_conf_test > uc_test;
		boost::shared_ptr< obj_dom_alias_test > da_test;
		boost::shared_ptr< obj_dom_ls_test > dom_ls_test;

		vq_test_suite(int ac, char *av[]) 
				: test_suite("qmailvqd tests"),
				test(new vq_test(ac, av)),
				uc_test(new obj_user_conf_test(test->vq)),
				da_test(new obj_dom_alias_test(test->vq)),
				dom_ls_test(new obj_dom_ls_test(test->vq)) {

			test_case * ts_init = BOOST_CLASS_TEST_CASE( 
				&vq_test::init, test );
			add(ts_init);

			test_case * ts_case1 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case1, test );
			ts_case1->depends_on(ts_init);
			add(ts_case1);

			test_case * ts_case2 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case2, test );
			ts_case2->depends_on(ts_init);
			add(ts_case2);

			test_case * ts_case3 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case3, test );
			ts_case3->depends_on(ts_init);
			add(ts_case3);

			test_case * ts_case_dom_name1 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case_dom_name1, test );
			ts_case_dom_name1->depends_on(ts_init);
			add(ts_case_dom_name1);

			test_case * ts_case4 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case4, test );
			ts_case4->depends_on(ts_init);
			add(ts_case4);

			test_case * ts_case5 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case5, test );
			ts_case5->depends_on(ts_init);
			add(ts_case5);

			test_case * ts_case6 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case6, test );
			ts_case6->depends_on(ts_init);
			add(ts_case6);
#if 0		
			test_case * ts_case7 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case7, test );
			ts_case7->depends_on(ts_case6);
			add(ts_case7);

			test_case * ts_case8 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case8, test );
			ts_case8->depends_on(ts_case6);
			add(ts_case8);
#endif // if 0

			test_case * ts_case9 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case9, test );
			ts_case9->depends_on(ts_init);
			add(ts_case9);

			test_case * ts_case10 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case10, test );
			ts_case10->depends_on(ts_init);
			add(ts_case10);

			test_case * ts_case11 = BOOST_CLASS_TEST_CASE( 
				&vq_test::case11, test );
			ts_case11->depends_on(ts_init);
			add(ts_case11);

			test_case * ts_uc1 = BOOST_CLASS_TEST_CASE( 
				&vq_test::uc_case1, test );
			ts_uc1->depends_on(ts_init);
			add(ts_uc1);

			test_case * ts_uc2 = BOOST_CLASS_TEST_CASE( 
				&vq_test::uc_case2, test );
			ts_uc2->depends_on(ts_init);
			add(ts_uc2);

			test_case * ts_uc_cleanup = BOOST_CLASS_TEST_CASE( 
				&vq_test::uc_cleanup, test );
			ts_uc_cleanup->depends_on(ts_init);
			add(ts_uc_cleanup);

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

			{
			test_case * ts_case7 = BOOST_CLASS_TEST_CASE( 
				&obj_dom_alias_test::case7, da_test );
			ts_case7->depends_on(ts_init);
			add(ts_case7);
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
	std::auto_ptr<test_suite> test(BOOST_TEST_SUITE("ivq tests"));

	register_exception_translator<vq::null_error>( &et_vq_null_error );
	register_exception_translator<vq::db_error>( &et_vq_db_error );
	register_exception_translator<vq::except>( &et_vq_except );
	register_exception_translator<CORBA::Exception>( &et_corba_exception );

	test->add(new vq_test_suite(ac, av));

    return test.release();
}
 
