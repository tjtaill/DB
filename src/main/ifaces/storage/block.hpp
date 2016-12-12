/*
 * block.hpp
 *
 *  Created on: Nov 21, 2009
 *      Author: Troy
 */
#include <cstddef>

#ifndef BLOCK_HPP_
#define BLOCK_HPP_
struct block {
	static const std::size_t nrecords = 10;
	virtual std::size_t memory_size() = 0;
	virtual ~block(){};
};

#endif /* BLOCK_HPP_ */
