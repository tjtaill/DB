/*
 * record_block.hpp
 *
 *  Created on: Nov 23, 2009
 *      Author: Troy
 */
#include "block.hpp"
#include <cstddef>

#ifndef RECORD_BLOCK_HPP_
#define RECORD_BLOCK_HPP_
template<typename R, std::size_t N = block::nrecords>
struct record_block : public block {
	static const std::size_t memory_required
		= N * sizeof(R);

	virtual std::size_t memory_size(){
		return memory_required;
	}

	R* records[N];

	record_block();

	virtual ~record_block();

};


#endif /* RECORD_BLOCK_HPP_ */
