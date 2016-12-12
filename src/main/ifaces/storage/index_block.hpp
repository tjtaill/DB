/*
 * index_block.hpp
 *
 *  Created on: Dec 4, 2009
 *      Author: Troy
 */

#include "storage/block.hpp"
#include "schema/attribute_types.hpp"
#include "buffer/memory_size.hpp"
#include <cstddef>
#include <sstream>
#include <limits>
#include <boost/array.hpp>
#include <algorithm>

#ifndef INDEX_BLOCK_HPP_
#define INDEX_BLOCK_HPP_


static const std::size_t TREE_LEVELS = 5;
// four only allows me to index ~4 million records a tree depth of 5
// allows me to index ~245 million records

// I wan't to be able to fit the depth of the tree index blocks in memory
// for splitting and searching purposes solve for N
static const std::size_t INDEX_N = (((MAX_MEMORY / TREE_LEVELS)
		- (sizeof(bool)) - (3 * sizeof(int_type)))
		/ (2 * sizeof(int_type))) - 1;

template<std::size_t N = INDEX_N>
struct index_block: public block {
	int_type block_number;
	int_type used;
	bool is_leaf;
	int_type keys[N];
	int_type blocks[N + 1];

	/*
	static const std::size_t memory_required = ((2 * N + 3) * sizeof(int_type))
			+ (sizeof(bool));
			*/

	static const std::size_t disk_size =
			+ INT_DISK_SIZE(block_number)
			+ INT_DISK_SIZE(used)
			+ BOOL_DISK_SIZE(is_leaf)
			+ (N * INT_DISK_SIZE(keys))
			+ ((N + 1) * INT_DISK_SIZE(blocks))
			+ NEWLINE_DISK_SIZE;


	static index_block<N>* deserialize(const char* const stream) {
		index_block<N>* ib = new index_block<N> ();
		std::size_t bytes_read = 0;
		ib->block_number = deserialize_int(stream, bytes_read);
		ib->used = deserialize_int(stream, bytes_read);
		ib->is_leaf = deserialize_bool(stream, bytes_read);
		for (std::size_t i = 0; i < N; i++) {
			ib->keys[i] = deserialize_int(stream, bytes_read);
			ib->blocks[i] = deserialize_int(stream, bytes_read);
		}
		ib->blocks[N] = deserialize_int(stream, bytes_read);
		return ib;
	}


	static std::string serialize(const index_block<N>* ib) {
		std::stringstream ss;
		ss << serialize_int(ib->block_number);
		ss << serialize_int(ib->used);
		ss << serialize_bool(ib->is_leaf);
		for (std::size_t i = 0; i < N; i++) {
			ss << serialize_int(ib->keys[i]);
			ss << serialize_int(ib->blocks[i]);
		}
		ss << serialize_int(ib->blocks[N]);
		ss << serialize_newline();
		return ss.str();
	}

	virtual std::size_t memory_size();

	std::size_t level(){
		std::size_t nodes = 1;
		std::size_t level = 1;
		if ( block_number < nodes )
			return 1;
		nodes = (N + 1);
		while(1){
			level++;
			if ( block_number < nodes + 1 )
				return level;
			nodes *= N + 1;
		}
	}

	index_block(){
		block_number = -1;
		used = 0;
		is_leaf = true;
		for (std::size_t i = 0; i < N; i++){
			keys[i] = std::numeric_limits<int_type>::max();
			blocks[i] = -1;
		}
		blocks[N] = -1;
	}

	index_block( index_block<N>& ib){
			block_number = ib.block_number;
			used = ib.used;
			is_leaf = ib.is_leaf;
			std::copy(&(ib.keys[0]), &(ib.keys[N]), keys);
			std::copy(&(ib.blocks[0]), &(ib.blocks[N+1]), blocks);
	}

	virtual ~index_block();
};

#endif /* INDEX_BLOCK_HPP_ */
