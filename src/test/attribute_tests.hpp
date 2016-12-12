/*
 * deserialization_tests.cpp
 *
 *  Created on: Nov 22, 2009
 *      Author: Troy
 */

#include "schema/attribute_types.hpp"

BOOST_AUTO_TEST_CASE(test_deserialize_int){
	const char stream[] = "42         ";
	size_t bytes_read = 0;
	BOOST_CHECK_EQUAL(42, deserialize_int(stream, bytes_read));
	BOOST_CHECK_EQUAL(11, bytes_read);
}

BOOST_AUTO_TEST_CASE(test_deserialize_float){
	const char stream[] = "3.14                  ";
	size_t bytes_read = 0;
	float_type expected = 3.14;
	float_type tolerance = 0.001;
	BOOST_CHECK_CLOSE(expected,
			deserialize_float(stream, bytes_read), tolerance);
	BOOST_CHECK_EQUAL(22, bytes_read);
}

BOOST_AUTO_TEST_CASE(test_deserialize_char){
	const char stream[] = "Troy Taillefer";
	char buffer[10];
	size_t bytes_read = 5;
	size_t bytes_to_read = 9;
	deserialize_char(stream, buffer, bytes_read, bytes_to_read);


	BOOST_CHECK_EQUAL("Taillefer", buffer);
	BOOST_CHECK_EQUAL(14, bytes_read);
}

BOOST_AUTO_TEST_CASE(test_string_to_date){
	const char date[] = "12/1/1996 0:00:00 ";
	posix_time::ptime time = mdy_string_to_date(date);
	gregorian::date d = time.date();
	posix_time::time_duration td = time.time_of_day();

	BOOST_CHECK_EQUAL(1996, d.year());
	BOOST_CHECK_EQUAL(12, d.month());
	BOOST_CHECK_EQUAL(1, d.day());

	BOOST_CHECK_EQUAL(0, td.hours());
	BOOST_CHECK_EQUAL(0, td.minutes());
	BOOST_CHECK_EQUAL(0, td.seconds());
}

BOOST_AUTO_TEST_CASE(test_deserialize_date){
	const char date[] = "12/1/1996 0:00:00 ";
	size_t bytes_read = 0;
	std::time_t t = deserialize_mdy_date( date, bytes_read);
	BOOST_CHECK_EQUAL(849416400, t);
}



