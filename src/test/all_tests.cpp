/*
 * all_tests.cpp
 *
 *  Created on: Nov 23, 2009
 *      Author: Troy
 */

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>


#include <cstddef>

#include <string>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian_calendar.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <ctime>


using namespace std;
using namespace boost;
using namespace boost::algorithm;

#include "attribute_tests.hpp"
#include "customer_tests.hpp"
#include "storage_manager_tests.hpp"
#include "buffer_manager_tests.hpp"
#include "line_item_tests.hpp"
#include "index_tests.hpp"


