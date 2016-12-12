/*
 * storage_manager.hpp
 *
 *  Created on: Nov 21, 2009
 *      Author: Troy
 */

#include <fstream>
#include <cassert>
#include <cstddef>
#include "storage/block.hpp"
#include "storage/index_block.hpp"
#include <iostream>

#ifndef STORAGE_MANAGER_HPP_
#define STORAGE_MANAGER_HPP_

static std::size_t blocks_ios = 0;

template<typename R, template<typename E, std::size_t F> class B, std::size_t N>
B<R, N>* load_record_block(const char* const file_name,
		unsigned int block_number) {
	B<R, N>* blk = new B<R, N> ();

	std::ifstream file(file_name);

	std::size_t position = block_number * R::disk_size * N;

	file.seekg(position);

	if (file.fail())
		return NULL;

	std::size_t records_read = 0;

	bool file_ended = file.eof();

	// check for end of file no then no such block exists
	if (file_ended)
		return NULL;

	R* rec;
	while (records_read < N && !file_ended) {
		rec = NULL;
		char buffer[R::disk_size - 1];

		file.getline(buffer, R::disk_size);
		// std::cout << "read : " << file.gcount() << " out of: " << R::disk_size - 1 <<std::endl;

		if (file.gcount() < R::disk_size - 1 && 0 == records_read) {
			delete blk;
			return NULL;
		}

		if (file.gcount() == R::disk_size - 1) {
			rec = R::deserialize(buffer);
		}

		blk->records[records_read++] = rec;
		file_ended = file.eof();
	}
	blocks_ios++;
	return blk;
}

template<typename R>
void write_record(std::ofstream& ostream, R* r) {
	if (NULL == r)
		return;
	ostream << R::serialize(r);
}

template<typename R, template<typename E, std::size_t F> class B, std::size_t N>
void write_record_block(std::ofstream& ostream, B<R, N>* blk) {
	if (NULL == blk)
		return;
	blocks_ios++;
	for (std::size_t i = 0; i < N; i++) {
		R* r = blk->records[i];
		write_record<R> (ostream, r);
	}
}

template<std::size_t N>
index_block<N>* load_index_block(const char* const index_file, int_type blockn) {
	std::size_t position = blockn * index_block<N>::disk_size;

	std::ifstream file(index_file);

	file.seekg(position);

	if (file.fail())
		return NULL;

	bool file_ended = file.eof();

	// check for end of file no then no such block exists
	if (file_ended)
		return NULL;

	char buffer[index_block<N>::disk_size - 1];

	file.getline(buffer, index_block<N>::disk_size);



	if ( file.gcount() != index_block<N>::disk_size - 1 )
		return NULL;
	blocks_ios++;
	return index_block<N>::deserialize( buffer );
}


template <std::size_t N>
void write_index_block(const char* const index_file, const index_block<N>* ib, bool create = false) {
	if ( NULL == ib ) return;

	std::size_t position = ib->block_number * index_block<N>::disk_size;

	std::fstream file;
	if ( create )
		file.open(index_file, std::ios::ate | std::ios::out );
	else
		file.open(index_file, std::ios::ate | std::ios::out | std::ios::in);


	file.seekp(position, std::ios::beg);

	if ( file.fail() ){
		std::cout << "failed to seek to " << position << " for block " << ib->block_number << std::endl;
		return;
	}



	std::string ss = index_block<N>::serialize(ib);
	// std::cout << ss << std::endl;
	file << ss;

	blocks_ios++;
}

template <std::size_t N>
void write_index_blockn(const char* const index_file, const index_block<N>* ib, std::size_t blkn) {
	if ( NULL == ib ) return;

	std::size_t position = blkn * index_block<N>::disk_size;

	std::fstream file;
	file.open(index_file, std::ios::ate | std::ios::out | std::ios::in);


	file.seekp(position, std::ios::beg);

	if ( file.fail() ){
		std::cout << "failed to seek to " << position << " for block " << ib->block_number << std::endl;
		return;
	}



	std::string ss = index_block<N>::serialize(ib);
	// std::cout << ss << std::endl;
	file << ss;

	blocks_ios++;
}




#endif /* STORAGE_MANAGER_HPP_ */
