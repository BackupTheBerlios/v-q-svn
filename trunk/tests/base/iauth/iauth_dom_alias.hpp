#ifndef __IAUTH_DA_HPP
#define __IAUTH_DA_HPP

#include "../../../base/auth.hpp"

#include <getlines.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>

#include <fstream>

template <class OBJ_TYPE>
struct da_dip_test {
		typedef std::deque< std::string > string_array; 
		typedef std::map< CORBA::String_var, std::string > id2name_map;
		typedef std::map< std::string, CORBA::String_var > name2id_map;

		vq::ivq::error_var err;

		OBJ_TYPE & obj;

		da_dip_test( OBJ_TYPE & o ) : obj(o) {
		}

		/**
		 * Test if da_add and dip_add returns err_exists if entry exists.
		 * Test also if removing domain removes aliases and IP addresses.
		 */
		void case4() {
			const char * dom = "case4.pl";
			CORBA::String_var dom_id;
			err = obj->dom_id(dom, dom_id);
			if( ::vq::ivq::err_noent == err->ec ) {
					IVQ_ERROR_EQUAL(obj->dom_add(dom, dom_id),
						::vq::ivq::err_no );
			}
			const char * ali = "alias2case4.pl";
			const char * ip = "10.1.1.16";
			IVQ_ERROR_EQUAL(obj->da_add(dom_id, ali), ::vq::ivq::err_no);
			IVQ_ERROR_EQUAL(obj->da_add(dom_id, ali), ::vq::ivq::err_exists);
			IVQ_ERROR_EQUAL(obj->dip_add(dom_id, ip), ::vq::ivq::err_no);
			IVQ_ERROR_EQUAL(obj->dip_add(dom_id, ip), ::vq::ivq::err_exists);

			IVQ_ERROR_EQUAL(obj->dom_rm(dom_id), ::vq::ivq::err_no);
			IVQ_ERROR_EQUAL(obj->dom_add(dom, dom_id), ::vq::ivq::err_no);
			IVQ_ERROR_EQUAL(obj->da_add(dom_id, ali), ::vq::ivq::err_no);
			IVQ_ERROR_EQUAL(obj->dip_add(dom_id, ip), ::vq::ivq::err_no);

			IVQ_ERROR_EQUAL(obj->dom_rm(dom_id), ::vq::ivq::err_no);
		}
		
		/**
		 * Test if dom_id resolves id. for IP addresses and aliases.
		 */
		void case5() {
			std::ifstream ifs("data/dom_alias/case5/insert");
			string_array insert;
			BOOST_REQUIRE( sys::getlines<std::string>(ifs, insert) );
			name2id_map dom2id;
			CORBA::String_var dom_id;
			string_array::const_iterator ib, ie, vb, ve;
			string_array fields, vals;
			for( ib=insert.begin(), ie=insert.end(); ib!=ie; ++ib ) {
					if( ib->empty() || '#' == (*ib)[0] ) continue;
					fields = text::split(*ib, "|");
					if( fields.empty() ) continue;
					
					err = obj->dom_id(fields[0].c_str(), dom_id);
					if( ::vq::ivq::err_noent == err->ec ) {
							err = obj->dom_add(fields[0].c_str(), dom_id);
					}
					BOOST_REQUIRE( ::vq::ivq::err_no == err->ec );
					dom2id[ fields[0] ] = dom_id;

					if( fields.size() == 1 ) continue;
					vals = text::split(fields[1], ",");
					for( vb=vals.begin(), ve=vals.end(); vb!=ve; ++vb ) {
							err = obj->da_add(dom_id, vb->c_str());
							BOOST_REQUIRE(::vq::ivq::err_no == err->ec
								|| ::vq::ivq::err_exists == err->ec );
					}
					if( fields.size() == 2 ) continue;
					vals = text::split(fields[2], ",");
					for( vb=vals.begin(), ve=vals.end(); vb!=ve; ++vb ) {
							err = obj->dip_add(dom_id, vb->c_str());
							BOOST_REQUIRE(::vq::ivq::err_no == err->ec
								|| ::vq::ivq::err_exists == err->ec );
					}
			}
			ifs.close();
			ifs.clear();
			ifs.open( "data/dom_alias/case5/check" );
			string_array check;
			BOOST_REQUIRE( sys::getlines<std::string>(ifs, check) );
			name2id_map::const_iterator ni, ne = dom2id.end();
			for( ib=check.begin(), ie=check.end(); ib!=ie; ++ib ) {
					if( ib->empty() || '#' == (*ib)[0] ) continue;
					fields = text::split(*ib, "|");
					if( fields.size() != 2 ) continue;
					IVQ_ERROR_EQUAL(obj->dom_id(fields[0].c_str(), dom_id),
						::vq::ivq::err_no );
					
					ni = dom2id.find(fields[1]);
					if( ni != ne )
							BOOST_CHECK(!strcmp(ni->second, dom_id));
			}
			for( ni=dom2id.begin(), ne=dom2id.end(); ni!=ne; ++ni ) {
					IVQ_ERROR_EQUAL(obj->dom_rm(ni->second),
						::vq::ivq::err_no );
			}
		}
		 
		/**
		 * add IP addresses for domain created in case6
		 * get list of IP addresses for domain created in case6
		 * get list of IP addresses for not existing domain
		 * remove IP addresses
		 */
		void case6() {
			const char * dom = "case6.pl";

			std::ifstream ifs("data/dom_alias/ips");
			string_array ips;
			BOOST_REQUIRE( sys::getlines<std::string>(ifs, ips) );
			
			CORBA::String_var dom_id;
			err = obj->dom_id(dom, dom_id);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			BOOST_REQUIRE(*dom_id);
			for( string_array::value_type::size_type i=0, s=ips.size(); i<s; ++i ) {
					err = obj->dip_add(dom_id, ips[i].c_str());
					IVQ_ERROR_EQUAL(err, vq::ivq::err_no);
			}
			vq::iauth::string_list_var from_db, from_db1;
			err = obj->dip_ls_by_dom(dom_id, from_db);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			BOOST_CHECK_EQUAL(from_db->length(), ips.size());
			// need to compare all of them
			std::map<std::string, char> in_db;
			for( unsigned i=0, s=from_db->length(); i<s; ++i ) {
					in_db[static_cast<const char *>(from_db[i])] = 1;
			}
			for( string_array::value_type::size_type i=0, s=ips.size(); i<s; ++i ) {
					BOOST_CHECK_EQUAL(in_db[ ips[i] ], 1);
			}
		
			err = obj->dip_ls_by_dom(static_cast<const char *>("-1"), from_db1);
			IVQ_ERROR_EQUAL(err, vq::ivq::err_no );
			BOOST_CHECK_EQUAL(from_db1->length(), 0U);

			err = obj->dip_rm( ips[0].c_str() );
			IVQ_ERROR_EQUAL(err, vq::ivq::err_no );
			err = obj->dip_ls_by_dom(dom_id, from_db);
			IVQ_ERROR_EQUAL(err, vq::ivq::err_no);
			BOOST_CHECK_EQUAL(from_db->length(), ips.size()-1);

			for( string_array::value_type::size_type i=0, s=ips.size(); i<s; ++i ) {
					err = obj->dip_rm( ips[i].c_str() );
					IVQ_ERROR_EQUAL(err, vq::ivq::err_no);
			}
		}

		/**
		 * add aliases for domain created in case6
		 * get list of aliases for domain created in case6
		 * get list of aliases for not existing domain
		 * remove aliases
		 */
		void case7() {
			const char * dom = "case6.pl";
			const char * reas[] = {
				"test",
				"test.sdfsdfsdf",
				"oloo"
			};
			unsigned reas_cnt = sizeof(reas)/sizeof(*reas);
			CORBA::String_var dom_id;
			err = obj->dom_id(dom, dom_id);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			BOOST_REQUIRE(*dom_id);
			for( unsigned i=0; i<reas_cnt; ++i ) {
					err = obj->da_add(dom_id, reas[i]);
					IVQ_ERROR_EQUAL(err, vq::ivq::err_no);
			}
			vq::iauth::string_list_var from_db, from_db1;
			err = obj->da_ls_by_dom(dom_id, from_db);
			BOOST_CHECK_EQUAL(err->ec, vq::ivq::err_no);
			BOOST_CHECK_EQUAL(from_db->length(), reas_cnt);
			// need to compare all of them
			std::map<std::string, char> in_db;
			for( unsigned i=0, s=from_db->length(); i<s; ++i ) {
					in_db[static_cast<const char *>(from_db[i])] = 1;
			}
			for( unsigned i=0; i<reas_cnt; ++i ) {
					BOOST_CHECK_EQUAL(in_db[reas[i]], 1);
			}
		
			err = obj->da_ls_by_dom(static_cast<const char *>("-1"), from_db1);
			IVQ_ERROR_EQUAL(err, vq::ivq::err_no );
			BOOST_CHECK_EQUAL(from_db1->length(), 0U);

			err = obj->da_rm(reas[0]);
			IVQ_ERROR_EQUAL(err, vq::ivq::err_no );
			err = obj->da_ls_by_dom(dom_id, from_db);
			IVQ_ERROR_EQUAL(err, vq::ivq::err_no);
			BOOST_CHECK_EQUAL(from_db->length(), reas_cnt-1);

			for( unsigned i=0; i<reas_cnt; ++i ) {
					err = obj->da_rm(reas[i]);
					IVQ_ERROR_EQUAL(err, vq::ivq::err_no);
			}
		}

}; // struct dom_alias_test

#endif // ifndef __IAUTH_DA_HPP
