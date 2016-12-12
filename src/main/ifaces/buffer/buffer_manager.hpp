/*
 * buffer_manager.hpp
 *
 *  Created on: Nov 25, 2009
 *      Author: Troy
 */
#include <cstddef>
#include <boost/unordered_map.hpp>
#include <boost/functional.hpp>
#include "buffer_key.hpp"
#include "buffer.hpp"
#include "storage/block.hpp"
#include <vector>
#include <boost/foreach.hpp>
#include <limits>
#include "storage/storage_manager.hpp"
#include <utility>
#include "buffer/memory_size.hpp"
#include "storage/index_block.hpp"
#include <iostream>

#ifndef BUFFER_MANAGER_HPP_
#define BUFFER_MANAGER_HPP_
struct buffer_manager {
	static const std::size_t max_memory_size = MAX_MEMORY;
	std::size_t memory_used;

	buffer_manager() {
		memory_used = 0;
	}

	typedef boost::unordered_map<buffer_key, buffer*, boost::hash<buffer_key> >
			buffer_map;

	buffer_map buffers;

	void make_space(std::size_t memory_required) {
		while (available_memory() < memory_required) {
			buffer* to_evict;
			date_type min_date = std::numeric_limits<date_type>::max();
			BOOST_FOREACH(buffer_map::value_type e, buffers){
				if ( min_date > e.second->time_stamp ) {
				min_date = e.second->time_stamp;
				to_evict = e.second;
			}
		}
		memory_used -= to_evict->memory_size();
		buffers.erase( to_evict->key );
		delete to_evict;
	}
}

template<typename R,
template< typename E, std::size_t F> class B,
std::size_t N>
B<R, N>* access_record_block(buffer_key k) {
	// check if it is in memory
	buffer_map::iterator pos =
	buffers.find( k );
	if ( pos != buffers.end() ) {
		return dynamic_cast< B<R,N>* >(pos->second->access()); // if so just return it
	}

	if ( B<R,N>::memory_required < available_memory()) {
		make_space(B<R,N>::memory_required);
	}

	// now read it from disk
	B<R,N>* blk
	= load_record_block<R,B,N>( k.file_name.c_str(),
			k.block_number );

	if ( NULL == blk ) return NULL;

	buffer* buf = new buffer(k, blk);

	memory_used += B<R,N>::memory_required;

	buffers.insert( std::make_pair(k, buf));
	return blk;
}

template<std::size_t N>
index_block<N>* access_index_block(buffer_key k) {
	// check if it is in memory
	buffer_map::iterator pos =
	buffers.find( k );
	if ( pos != buffers.end() ) {
		return dynamic_cast< index_block<N>* >(pos->second->access()); // if so just return it
	}

	if ( sizeof(index_block<N>) < available_memory()) {
		make_space(sizeof(index_block<N>));
	}

	// now read it from disk
	index_block<N>* blk
	= load_index_block<N>( k.file_name.c_str(),
			k.block_number );

	if ( NULL == blk ) return NULL;

	buffer* buf = new buffer(k, blk);

	memory_used += sizeof(index_block<N>);

	buffers.insert( std::make_pair(k, buf));
	return blk;
}

~buffer_manager() {
	BOOST_FOREACH(buffer_map::value_type e, buffers) {
		delete e.second;
	}
}

std::size_t available_memory() {
	return max_memory_size - memory_used;
}

void remove_buffers() {
	BOOST_FOREACH(buffer_map::value_type e, buffers){
		memory_used -= e.second->memory_size();
		buffers.erase(e.second->key);
		delete e.second;
	}
}

};

static buffer_manager buf_manager;

#endif /* BUFFER_MANAGER_HPP_ */
