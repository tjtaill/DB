/*
 * customer_tests.hpp
 *
 *  Created on: Nov 23, 2009
 *      Author: Troy
 */

#ifndef CUSTOMER_TESTS_HPP_
#define CUSTOMER_TESTS_HPP_
#include "schema/customer.hpp"

BOOST_AUTO_TEST_CASE(test_deserialize_customer){

	char stream[] = "1177       Customer#000001177       hZPNQ8a9QRM ,SYdTdoW9hw                           14         24-200-701-8606     9281.72               FURNITURE      furiously regular asymptotes against the regul                                                                          ";
	customer* c = customer::deserialize( stream );
	BOOST_CHECK_EQUAL(1177, c->key);
	BOOST_CHECK_EQUAL("Customer#000001177",
			trim_right_copy(string(c->name)));
	BOOST_CHECK_EQUAL("hZPNQ8a9QRM ,SYdTdoW9hw",
			trim_right_copy(string(c->address)));
	BOOST_CHECK_EQUAL(14, c->nation_key);
	BOOST_CHECK_EQUAL("24-200-701-8606",
			trim_right_copy(string(c->phone_number)));
	float_type expected = 9281.72;
	float_type tolerance = 0.001;
	BOOST_CHECK_CLOSE(expected, c->account_balance, tolerance );
	BOOST_CHECK_EQUAL("FURNITURE",
			trim_right_copy(string(c->market_segment)));
	BOOST_CHECK_EQUAL("furiously regular asymptotes against the regul",
			trim_right_copy(string(c->comment)));

}




#endif /* CUSTOMER_TESTS_HPP_ */
