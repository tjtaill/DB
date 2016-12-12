/*
 * buffer_manager_tests.hpp
 *
 *  Created on: Nov 25, 2009
 *      Author: Troy
 */

#include "storage/record_block.hpp"
#include "schema/customer.hpp"
#include "storage/block.hpp"
#include "buffer/buffer_manager.hpp"

#ifndef BUFFER_MANAGER_TESTS_HPP_
#define BUFFER_MANAGER_TESTS_HPP_

static buffer_manager m;

BOOST_AUTO_TEST_CASE(test_access_block){
	int_type keys[10] = {
			1177,
			1646,
			2537,
			3478,
			3590,
			8515,
			10876,
			11255,
			13306,
			14365
	};



	char file_name[] =
			"C:\\Code\\Eclipse3.5\\Comp6521b\\data\\short\\Customer.txt";

	buffer_key k(file_name, 0);



	record_block<customer>* cblk
			= m.access_record_block<customer, record_block, block::nrecords>(k);

	for(std::size_t i = 0; i < block::nrecords; i++){
		BOOST_CHECK_EQUAL(keys[i], cblk->records[i]->key);
	}

	BOOST_CHECK_EQUAL(cblk->memory_size(), m.memory_used);
}


BOOST_AUTO_TEST_CASE(test_access_block2){
	int_type keys[10] = {
			31696,
			32185,
			34624,
			35111,
			35273,
			36887,
			38299,
			40321,
			42754,
			45805
	};



	char file_name[] =
			"C:\\Code\\Eclipse3.5\\Comp6521b\\data\\short\\Customer.txt";

	buffer_key k(file_name, 2);

	record_block<customer>* cblk
			= m.access_record_block<customer, record_block, block::nrecords>(k);

	for(std::size_t i = 0; i < block::nrecords; i++){
		BOOST_CHECK_EQUAL(keys[i], cblk->records[i]->key);
	}

	BOOST_CHECK_EQUAL(2 * cblk->memory_size(), m.memory_used);
}
#endif /* BUFFER_MANAGER_TESTS_HPP_ */
