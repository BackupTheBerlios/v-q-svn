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
#include <algorithm>

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
		typedef std::deque< string_array > fields_array;

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

		/**
		 * Removing all entries and call all functions that retrieve
		 * informations from log.
		 */
		void case2() {
			::vq::ilogger::size_type cnt;
			IVQ_ERROR_EQUAL(obj->rm_all(), ::vq::ivq::err_no);
			IVQ_ERROR_EQUAL(obj->count(cnt), ::vq::ivq::err_no);
			BOOST_CHECK_EQUAL(cnt, 0U);
	
			BOOST_CHECK_NO_THROW(obj->clear());
			IVQ_ERROR_EQUAL(obj->count_by_dom(cnt), ::vq::ivq::err_no);
			BOOST_CHECK_EQUAL(cnt, 0U);
			IVQ_ERROR_EQUAL(obj->count_by_user(cnt), ::vq::ivq::err_no);
			BOOST_CHECK_EQUAL(cnt, 0U);
			
			BOOST_CHECK_NO_THROW(obj->domain_set("Asdasd"));
			BOOST_CHECK_NO_THROW(obj->login_set("Asdasd"));
			IVQ_ERROR_EQUAL(obj->count_by_dom(cnt), ::vq::ivq::err_no);
			BOOST_CHECK_EQUAL(cnt, 0U);
			IVQ_ERROR_EQUAL(obj->count_by_user(cnt), ::vq::ivq::err_no);
			BOOST_CHECK_EQUAL(cnt, 0U);
		}

		/**
		 * Reset object. Write log without setting any properties.
		 * Read log. Remove entries.
		 */
		void case3() {
			BOOST_CHECK_NO_THROW(obj->clear());
			IVQ_ERROR_EQUAL(obj->write(::vq::ilogger::re_unknown, ""), 
				::vq::ivq::err_no);
			IVQ_ERROR_EQUAL(obj->write(::vq::ilogger::re_ok, "re_ok"), 
				::vq::ivq::err_no);
			IVQ_ERROR_EQUAL(obj->write(::vq::ilogger::re_read, "re_read"), 
				::vq::ivq::err_no);
			::vq::ilogger::size_type cnt;
			IVQ_ERROR_EQUAL(obj->count(cnt), ::vq::ivq::err_no);
			BOOST_CHECK_EQUAL(cnt, 3U);
			
			::vq::ilogger::log_entry_list_var les;
			IVQ_ERROR_EQUAL(obj->read(0, 0, les), ::vq::ivq::err_no);

			CORBA::ULong s = les->length();
			BOOST_CHECK_EQUAL(s, 3U);
			if( s > 0U ) {
					BOOST_CHECK(!strcmp(les[0U].msg, "re_read"));
					BOOST_CHECK_EQUAL(les[0U].res, ::vq::ilogger::re_read);

					if( s > 1U ) {
							BOOST_CHECK(!strcmp(les[1U].msg, "re_ok"));
							BOOST_CHECK_EQUAL(les[1U].res, ::vq::ilogger::re_ok);

							if( s > 2U ) {
									BOOST_CHECK(!strcmp(les[2U].msg, ""));
									BOOST_CHECK_EQUAL(les[2U].res, 
										::vq::ilogger::re_unknown);
							}
					}
			}
			IVQ_ERROR_EQUAL(obj->rm_all(), ::vq::ivq::err_no);
		}

		/**
		 *
		 */

		void fields_log_entry_cmp( const string_array & fb, 
				const ::vq::ilogger::log_entry & les, 
				bool dom_ign = false, bool log_ign = false) {

			string_array::size_type idx=0;
			BOOST_CHECK_EQUAL( fb[idx++], static_cast<const char *>(les.ip) );
			BOOST_CHECK_EQUAL( fb[idx++], 
				boost::lexical_cast<std::string>(les.ser) );
			if( ! dom_ign )
					BOOST_CHECK_EQUAL( fb[idx], 
						static_cast<const char *>(les.domain) );
			++idx;
			if( ! log_ign )
					BOOST_CHECK_EQUAL( fb[idx], 
						static_cast<const char *>(les.login) );
			++idx;
			BOOST_CHECK_EQUAL( fb[idx++],
				boost::lexical_cast<std::string>(les.res) );
			BOOST_CHECK_EQUAL( fb[idx++], static_cast<const char *>(les.msg) );
		}

		/**
		 * Reset object. Write logs based on file. Use read, 
		 * read_by_dom, read_by_user
		 */
		void case4() {
			typedef std::map< std::string, fields_array > doms_map_type;
			typedef std::map< std::pair<std::string, std::string>, 
					fields_array > users_map_type;
				
			BOOST_CHECK_NO_THROW(obj->rm_all());
				
			std::ifstream ifs("data/case4/logs");
			string_array logs;
			BOOST_REQUIRE(sys::getlines<std::string>(ifs, logs));
			ifs.close();
			ifs.clear();
			fields_array fields;
			std::transform( logs.begin(), logs.end(), 
				std::back_inserter(fields),
				std::bind2nd( text::split_t(), "|") );

			fields_array::const_iterator fb, fe;
			CORBA::ULong wcnt;
			doms_map_type doms_map;
			users_map_type users_map;
			for( wcnt = 0, fb=fields.begin(), fe=fields.end(); fb!=fe; ++fb ) {
					if( fb->size() < 6 || 
						( ! (*fb)[0].empty() && '#' == (*fb)[0][0] ) ) continue;
					
					BOOST_CHECK_NO_THROW(obj->clear());
					BOOST_CHECK_NO_THROW(obj->ip_set((*fb)[0].c_str()));
					BOOST_CHECK_NO_THROW(obj->service_set(
						boost::lexical_cast< ::vq::ilogger::service_type >
							((*fb)[1]) ));
					BOOST_CHECK_NO_THROW(obj->domain_set((*fb)[2].c_str()));
					BOOST_CHECK_NO_THROW(obj->login_set((*fb)[3].c_str()));
					IVQ_ERROR_EQUAL(obj->write( 
						boost::lexical_cast< ::vq::ilogger::result_type >
							( (*fb)[4] ), (*fb)[5].c_str() ), ::vq::ivq::err_no);
					++wcnt;

					// create maps for calling read_by_dom, read_by_user
					doms_map[(*fb)[2]].push_back(*fb);
					users_map[ std::make_pair((*fb)[2], (*fb)[3]) ].push_back(*fb);
			}

			::vq::ilogger::log_entry_list_var les;
			IVQ_ERROR_EQUAL(obj->read(0, 0, les), ::vq::ivq::err_no);
			CORBA::ULong rcnt = les->length(), i;

			BOOST_CHECK_EQUAL( rcnt, wcnt );
			for( i=1, fb=fields.begin(), fe=fields.end(); 
						rcnt == wcnt && fb!=fe; ++fb ) {
					if( fb->size() < 6 || 
						( ! (*fb)[0].empty() && '#' == (*fb)[0][0] ) ) continue;
					fields_log_entry_cmp(*fb, les[rcnt-i]);
					++i;
			}

			// check read_by_dom
			BOOST_CHECK_NO_THROW(obj->clear());
			for( doms_map_type::const_iterator db=doms_map.begin(), de=doms_map.end();
						db != de; ++db ) {
					
					BOOST_CHECK_NO_THROW(obj->domain_set(db->first.c_str()));
					IVQ_ERROR_EQUAL(obj->read_by_dom(0, 0, les), ::vq::ivq::err_no);
					rcnt = les->length();

					wcnt = db->second.size();
					BOOST_CHECK_EQUAL( rcnt, wcnt );
					for( i=1, fb=db->second.begin(), fe=db->second.end(); 
								rcnt == wcnt && fb!=fe; ++fb ) {
							fields_log_entry_cmp(*fb, les[rcnt-i], true);
							++i;
					}
			}

			// check read_by_user
			BOOST_CHECK_NO_THROW(obj->clear());
			for( users_map_type::const_iterator db=users_map.begin(), de=users_map.end();
						db != de; ++db ) {
					
					BOOST_CHECK_NO_THROW(obj->domain_set(db->first.first.c_str()));
					BOOST_CHECK_NO_THROW(obj->login_set(db->first.second.c_str()));
					IVQ_ERROR_EQUAL(obj->read_by_user(0, 0, les), ::vq::ivq::err_no);
					rcnt = les->length();

					wcnt = db->second.size();
					BOOST_CHECK_EQUAL( rcnt, wcnt );
					for( i=1, fb=db->second.begin(), fe=db->second.end(); 
								rcnt == wcnt && fb!=fe; ++fb ) {
							fields_log_entry_cmp(*fb, les[rcnt-i], true, true);
							++i;
					}
			}

			BOOST_CHECK_NO_THROW(obj->rm_all());
		}

		/**
		 * Check if start and count arguments in read functions are
		 * supported correctly.
		 */
		void case5() {
			BOOST_CHECK_NO_THROW(obj->clear());
			BOOST_CHECK_NO_THROW(obj->rm_all());

			CORBA::ULong i, j, si, limit = 50, steps[] = {1, 3, 5, 10, 20, 50, 70};
			CORBA::ULong stepsl = sizeof steps/sizeof *steps;
			for( i=0; i<limit; ++i ) {
					IVQ_ERROR_EQUAL(obj->write(::vq::ilogger::re_unknown, 
						boost::lexical_cast<std::string>(i).c_str()), 
						::vq::ivq::err_no);
			}

			::vq::ilogger::log_entry_list_var les;
			for( si=0; si<stepsl; ++si ) {
					for( i=0; i<limit; i+= steps[si] ) {
							IVQ_ERROR_EQUAL(obj->read(i, steps[si], les),
								::vq::ivq::err_no );
							
							for( j=i; j<steps[si] && j<limit; ++j ) {
									BOOST_CHECK_EQUAL( 
										boost::lexical_cast<std::string>
											(limit-j-1),
										static_cast<const char *>
											(les[j-i].msg));
							}
					}
			}

			BOOST_CHECK_NO_THROW(obj->rm_all());
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

			test_case * ts_case2 = BOOST_CLASS_TEST_CASE(
				&logger_test::case2, test );
			ts_case2->depends_on(ts_init);
			add(ts_case2);

			test_case * ts_case3 = BOOST_CLASS_TEST_CASE(
				&logger_test::case3, test );
			ts_case3->depends_on(ts_init);
			add(ts_case3);

			test_case * ts_case4 = BOOST_CLASS_TEST_CASE(
				&logger_test::case4, test );
			ts_case4->depends_on(ts_init);
			add(ts_case4);

			test_case * ts_case5 = BOOST_CLASS_TEST_CASE(
				&logger_test::case5, test );
			ts_case5->depends_on(ts_init);
			add(ts_case5);

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
 
