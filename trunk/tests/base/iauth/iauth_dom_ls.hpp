#ifndef __IAUTH_DOM_LS_HPP
#define __IAUTH_DOM_LS_HPP

#include "../../../base/auth.hpp"

#include <getlines.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>

#include <fstream>

template <class OBJ_TYPE>
struct dom_ls_test {
		typedef std::deque< std::string > string_array;

		vq::ivq::error_var err;

		OBJ_TYPE & obj;

		dom_ls_test( OBJ_TYPE & o ) : obj(o) {
		}

		/**
		 * add regex aliases for domain created in case6
		 * get list of regex aliases for domain created in case6
		 * get list of regex aliases for not existing domain
		 * remove regex aliases by pair: id_domain+re_alias
		 * remove regex aliases by id_domain
		 */
		void case1() {
			string_array doms, doms_sorted;
			std::ifstream ifs("data/dom_ls/domains");
			BOOST_REQUIRE(sys::getlines<std::string>(ifs, doms));
			ifs.close(); 
			ifs.clear();
			ifs.open("data/dom_ls/domains_sorted");
			BOOST_REQUIRE(sys::getlines<std::string>(ifs, doms_sorted));
			BOOST_REQUIRE(doms.size() == doms_sorted.size());
	
			::vq::ivq::id_type dom_id = ::vq::ivq::id_type();
			string_array::const_iterator di, de;
			for( di=doms.begin(), de=doms.end(); di!=de; ++di ) {
					err = obj->dom_add(di->c_str(), dom_id);
					BOOST_CHECK( ::vq::ivq::err_no == err->ec
						|| ::vq::ivq::err_exists == err->ec );
			}

			::vq::ivq::domain_info_list_var dis;
			IVQ_ERROR_EQUAL( obj->dom_ls(dis), ::vq::ivq::err_no );
			CORBA::ULong i, s = dis->length();
			BOOST_CHECK( s >= doms.size() );
			for( i=0, di=doms_sorted.begin(), de=doms_sorted.end(); 
						i<s && di != de; ++i ) {
					if( ! strcmp(dis[i].domain, di->c_str()) ) {
						++di;
					}
					IVQ_ERROR_EQUAL(obj->dom_rm(dis[i].id_domain),
						::vq::ivq::err_no );
			}
			BOOST_CHECK(di == de);
		}

}; // struct dom_alias_test

#endif // ifndef __IAUTH_DOM_LS_HPP
