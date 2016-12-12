/*
 * line_item_tests.hpp
 *
 *  Created on: Nov 26, 2009
 *      Author: Troy
 */
#include "schema/line_item.hpp"
#ifndef LINE_ITEM_TESTS_HPP_
#define LINE_ITEM_TESTS_HPP_


BOOST_AUTO_TEST_CASE(test_deserialize_line_item){
	char stream[] = "800        71833      6848       1          38         68583.54              0.00                  0.05                  N O 1998-7-21 0:00:00  1998-9-25 0:00:00  1998-8-7 0:00:00   TAKE BACK RETURN    TRUCK     quickly special requests                                                                                                ";
	line_item* r = line_item::deserialize( stream );


	BOOST_CHECK_EQUAL(800, r->order_key);
	BOOST_CHECK_EQUAL(71833, r->part_key);
	BOOST_CHECK_EQUAL(6848, r->supplier_key);
	BOOST_CHECK_EQUAL(1, r->line_number);
	BOOST_CHECK_EQUAL(38, r->quantity);
	BOOST_CHECK_CLOSE(68583.54, r->extended_price, 0.01);
	BOOST_CHECK_CLOSE(0.00, r->discount, 0.01);
	BOOST_CHECK_CLOSE(0.05, r->taxes, 0.01);
	BOOST_CHECK_EQUAL("N", r->return_flag);
	BOOST_CHECK_EQUAL("O", r->line_status);
	size_t br = 0;
	BOOST_CHECK_EQUAL(deserialize_ymd_date("1998-7-21 0:00:00  ", br),
			r->ship_date);
	br = 0;
	BOOST_CHECK_EQUAL(deserialize_ymd_date("1998-9-25 0:00:00  ", br),
				r->commit_date);
	br = 0;
	BOOST_CHECK_EQUAL(deserialize_ymd_date("1998-8-7 0:00:00   ", br),
					r->receipt_date);
	BOOST_CHECK_EQUAL("TAKE BACK RETURN", r->ship_instructions);
	BOOST_CHECK_EQUAL("TRUCK", r->ship_mode);
	BOOST_CHECK_EQUAL("quickly special requests", r->comment);
}

#endif /* LINE_ITEM_TESTS_HPP_ */
