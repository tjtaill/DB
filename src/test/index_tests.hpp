/*
 * index_tests.hpp
 *
 *  Created on: Dec 4, 2009
 *      Author: Troy
 */

#include "storage/index_block.hpp"
#include "storage/storage_manager.hpp"
#include "index/index_operations.hpp"
#include <fstream>
#include <set>
#include <cstddef>

#ifndef INDEX_TESTS_HPP_
#define INDEX_TESTS_HPP_

static const std::size_t b = 3;
static const int min_key = 1;
static const int max_key = 10;

BOOST_AUTO_TEST_CASE(test_index_block_level) {
	int_type blkns[] = {0, 3, 6, 11, 17};
	std::size_t levels[] = { 1, 2, 3, 3, 4};

	index_block<b> ib;
	for(int i = 0; i < 5; i++){
		ib.block_number = blkns[i];
		BOOST_CHECK_EQUAL(levels[i], ib.level() );
	}
}

/*
BOOST_AUTO_TEST_CASE(test_insert_desc_key) {
	std::string file(".\\data\\results\\tests\\desc.index");


	for(int i = max_key; i >= min_key; i--){
		insert_key<b>( file.c_str(), i, i);
	}

	// Troy the 3rd key was put in block 7 when it should have been in 8
	// figure it out
	for(int i = max_key; i >= min_key; i--){
		std::set<int_type> s = search_key<b>( file.c_str(), i);
		BOOST_CHECK_EQUAL(1, s.size());
		BOOST_CHECK_EQUAL(1, s.count(i));
	}

}
*/

BOOST_AUTO_TEST_CASE(test_insert_ascending_keys) {
	std::string file(".\\data\\results\\tests\\asc.index");

	for(int i = min_key; i <= max_key; i++){
		insert_key<b>( file.c_str(), i, i);
	}


	for(int i = min_key; i <= max_key; i++){
		std::set<int_type> s = search_key<b>( file.c_str(), i);
		BOOST_CHECK_EQUAL(1, s.size());
		BOOST_CHECK_EQUAL(1, s.count(i));
	}

}

#endif /* INDEX_TESTS_HPP_ */
