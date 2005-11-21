#ifndef KILL_MYSELF_HPP
#define KILL_MYSELF_HPP

#include <boost/test/unit_test.hpp>

struct kill_myself_suite : boost::unit_test_framework::test_suite {
		kill_myself_suite() : test_suite("kill in the name of the game") {
			test_case * ts_init = BOOST_TEST_CASE( 
				&kill_myself_suite::init );
			add(ts_init);
		}

		static void init() {
			alarm(1);
		}
};

#endif
