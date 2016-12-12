/*
 * buffer_key.hpp
 *
 *  Created on: Nov 25, 2009
 *      Author: Troy
 */

#include <string>
#include <cstddef>
#include <boost/functional/hash.hpp>

#ifndef BUFFER_KEY_HPP_
#define BUFFER_KEY_HPP_
struct buffer_key {
	std::string file_name;
	std::size_t block_number;
	buffer_key(){};

	buffer_key(std::string f, std::size_t b){
		file_name = f;
		block_number = b;
	};
};

bool operator==(buffer_key const& a, buffer_key const& b);
std::size_t hash_value(buffer_key const& key);


#endif /* BUFFER_KEY_HPP_ */
