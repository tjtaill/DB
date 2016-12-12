/*
 * buffer.hpp
 *
 *  Created on: Nov 25, 2009
 *      Author: Troy
 */

#include "storage/block.hpp"
#include "buffer_key.hpp"
#include "schema/attribute_types.hpp"
#include <cstddef>
#include <ctime>

#ifndef BUFFER_HPP_
#define BUFFER_HPP_
struct buffer {
	block* blk;
	date_type time_stamp;
	buffer_key key;

	std::size_t memory_size(){
		return blk->memory_size();
	}

	buffer(buffer_key k, block* b){
		key = k;
		time_stamp = std::time(NULL);
		blk = b;
	}

	block* access(){
		time_stamp = std::time(NULL);
		return blk;
	}

	bool operator<(buffer const& other){
		return this->time_stamp < other.time_stamp;
	}

	~buffer(){
		if ( NULL != blk )
			delete blk;
	}

};

#endif /* BUFFER_HPP_ */
