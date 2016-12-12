/*
 * physical_operators.hpp
 *
 *  Created on: Nov 25, 2009
 *      Author: Troy
 */

#include <cstddef>
#include "buffer/buffer_manager.hpp"
#include "buffer/buffer_key.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <set>
#include <cassert>
#include <cmath>
#include "storage/block.hpp"
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#ifndef PHYSICAL_OPERATORS_HPP_
#define PHYSICAL_OPERATORS_HPP_

template <typename R,
	template <typename E, std::size_t F> class B,
	std::size_t N>
void table_scan(const char* const file_name, void (*func)(B<R,N>*)){
	B<R,N>* blk;
	std::size_t nblock = 0;
	while(true){
		buffer_key key(file_name, nblock++);
		blk = buf_manager.access_record_block<R,B,N>( key );

		if ( NULL == blk )
			break;

		(*func)(blk);
	}

};

template <typename R,
	template <typename E, std::size_t F> class B,
	std::size_t N>
void filter_scan(const char* const in_file,
		const char* const out_file,
		bool (*filter)(const R*)){
	B<R,N>* blk;
	std::size_t nblock = 0;

	std::ofstream out(out_file);
	while(true){
		buffer_key key(in_file, nblock++);
		blk = buf_manager.access_record_block<R,B,N>( key );


		if ( NULL == blk ){
			return;
		}

		for(std::size_t i = 0; i < N; i++){
			if ( NULL == blk->records[i]  ) break;
			if ( (*filter)( blk->records[i] )){
				write_record<R>(out, blk->records[i]);
			}
		}
	}
};

template <typename R, typename AR,
	template <typename E, std::size_t F> class B>
void group_by( const char* const sorted_file,
		const char* const out_file,
		bool (*equal)(const R*, const R*),
		void (*accum)(const R*, AR*)){

	std::size_t cblockn = 0;

	B<R,1>* cblk;
	B<R,1>* sblk;

	std::ofstream out(out_file);
	do {
		AR ar;
		// read the current block
		buffer_key ckey(sorted_file, cblockn++);
		cblk = buf_manager.access_record_block<R,B,1>(ckey);

		if ( NULL == cblk ) break;

		(*accum)( cblk->records[0], &ar);

		// get the next block
		std::size_t sblockn = cblockn;
		buffer_key skey(sorted_file, sblockn++);
		B<R,1>* sblk = buf_manager.access_record_block<R,B,1>(skey);

		while( NULL != sblk && (*equal)(cblk->records[0], sblk->records[0] ) ){
			(*accum)(sblk->records[0], &ar);
			buffer_key nskey(sorted_file, sblockn++);
			sblk = buf_manager.access_record_block<R,B,1>(skey);
		}
		// write the merge record
		write_record<AR>( out, &ar );

		// finished merging sblockn points to the new location
		cblockn = sblockn - 1;
	} while(1);
}



template <typename R,
	template <typename E, std::size_t F> class B>
void sort_scan(const char* const source,
		const char* const destination,
		bool (*less)(const R*, const R*)){
	// caculate how many sublists
	// using namespace boost::numeric::converter;

	static const std::size_t max_block_size =
			buffer_manager::max_memory_size / sizeof(R);

	static const std::size_t max_blocks =
			( buffer_manager::max_memory_size / sizeof(R) ) - 1;

	B<R, max_block_size>* blk;
	std::size_t nblock = 0;


	boost::filesystem::path dest( destination );
	boost::filesystem::path dest_dir = dest.parent_path();
	boost::filesystem::path src( source );
	std::string src_name =
			boost::filesystem::basename( src );

	std::string extension = ".slst";

	std::set<std::string> sublist_paths;

	// phase one create sorted sublist
	while(true){
		buffer_key key(source, nblock++);

		blk =
			buf_manager.
				access_record_block<R,B, max_block_size>( key );

		if ( NULL == blk )
			break;

		// sort the block
		std::sort( &blk->records[0],
				&blk->records[max_block_size],
				less);

		// naming convention for sublist file names
		std::string number =
				boost::lexical_cast<std::string>( nblock );
		std::string final_name = src_name + number + extension;
		boost::filesystem::path
			final ( dest_dir / boost::filesystem::path(final_name) );

		sublist_paths.insert( final.string() );

		std::ofstream sublist( final.string().c_str() );

		// write the block out to the sublist
		write_record_block<R,B,max_block_size>(sublist, blk);
	}




	// calculate the number of merge rounds you need

	std::size_t merge_rounds =
			std::ceil( (double)sublist_paths.size() / max_blocks );
	B<R,1>* input_buffers[max_blocks];
	B<R,1>* output_buffer;
	std::size_t blocks_read[max_blocks];
	std::string infiles[max_blocks];

	for (std::size_t r = 0; r < merge_rounds; r++){
		for(std::size_t i = 0; i < max_blocks; i++){
			input_buffers[i] = NULL;
			blocks_read[i] = 0;
			infiles[i] = "";
		}

		std::ofstream out;

		// get the current sublists to merge
		typedef std::set<std::string>::iterator it_type;
		it_type	it(sublist_paths.begin()),
			end(sublist_paths.end());
		std::size_t nbuffers = 0;


		for(; nbuffers < max_blocks && it != end; nbuffers++ ){
			infiles[nbuffers] = *it;
			sublist_paths.erase( *(it++) );
		}


		bool round_done = false;

		// fill all input buffers

		for(std::size_t i = 0; i < nbuffers; i++){
			buffer_key k(infiles[i],
					blocks_read[i]++);
			input_buffers[i] =
					buf_manager.access_record_block<R,B,1>(k);
		}




		// naming convention for sublist file names
		std::string number =
			boost::lexical_cast<std::string>( r + 1 );
		std::string final_name =
				src_name + number + number + extension;
		boost::filesystem::path
			sub ( dest_dir / boost::filesystem::path(final_name) );

		if ( sublist_paths.empty() ){ // this is final output
			out.open( destination );
		} else { // just another sublist
			sublist_paths.insert( sub.string() );
			out.open( sub.string().c_str() );
		}

		while(1){
			// find the lowest value
			// detect if all input buffers are empty
			std::size_t nnulls = 0;
			std::size_t min = 0;
			for (std::size_t i = 0; i < nbuffers; i++){

				if ( NULL  == input_buffers[i] ){
					nnulls++;
					continue;
				}

				if ( NULL == input_buffers[min] ){
					min = i;
				} else if ( (*less)( input_buffers[i]->records[0],
						input_buffers[min]->records[0])){
					min = i;
				}
			}

			if ( nnulls == nbuffers ){
				round_done = true;
				break;
			}


			output_buffer = input_buffers[min];
			write_record_block<R,B,1>( out, output_buffer);

			// refill depleted input buffer
			buffer_key k(infiles[min], blocks_read[min]++);
			input_buffers[min] =
				buf_manager.access_record_block<R,B,1>(k);
		}
	}
}


template <typename A, typename R, typename S, typename J,
	template <typename E, std::size_t F> class B>
void sort_join(const char* const rfile,
		const char* const sfile,
		const char* const jfile,
		A (*jar)(const R*),
		A (*jas)(const S*),
		int (*cmp)(A, A),
		J* (*join)(const R*, const S*)){

	B<R,1>* r_inbuf;
	B<S,1>* s_inbuf;
	B<S,1>* s_scanbuf;
	B<J,1> outbuf;
	std::size_t rblock = 0, sblock = 0, scanb = 0;

	std::ofstream out( jfile );
	// fill input buffers
	buffer_key rk(rfile, rblock++);
	r_inbuf = buf_manager.access_record_block<R,B,1>(rk);

	buffer_key sk(sfile, sblock++);
	s_inbuf = buf_manager.access_record_block<S,B,1>(sk);

	while(1){
		if ( NULL == r_inbuf || NULL == s_inbuf ) break;

		A ra = (*jar)(r_inbuf->records[0]);
		A sa = (*jas)(s_inbuf->records[0]);

		int order = (*cmp)( ra, sa);
		if (0 == order ){ // S and R equal
			// do some joining

			A nra;
			A nsa;
			do {
				scanb = sblock;
				s_scanbuf = s_inbuf;
				do{ // scan down S joining any equal values
					J* jr = (*join)(r_inbuf->records[0],
							s_scanbuf->records[0]);

					outbuf.records[0] = jr;
					write_record_block<J,B,1>(out, &outbuf);

					buffer_key skey(sfile, scanb++);
					s_scanbuf = buf_manager.
							access_record_block<S,B,1>(skey);

					if ( NULL != s_scanbuf )
						nsa = (*jas)( s_scanbuf->records[0] );

				} while( NULL != s_scanbuf && 0 == (*cmp)(ra, nsa) );
				// remove the first R value
				buffer_key rkey(rfile, rblock++);
				r_inbuf = buf_manager.
					access_record_block<R,B,1>(rkey);
				if ( NULL != r_inbuf )
					nra = (*jar)( r_inbuf->records[0] );
			} while( NULL != r_inbuf && 0 == (*cmp)(ra, nra) );
			// now remove the S values with same value as R;
			do {
				buffer_key skey(sfile, sblock++);
				s_inbuf = buf_manager.
					access_record_block<S,B,1>(skey);
				if ( NULL != s_inbuf )
					nsa = (*jas)( s_inbuf->records[0] );
			} while( NULL != s_inbuf && 0 == (*cmp)(ra, nsa) );
		} else if ( 1 == order ) { // R is greater
			// remove some values from S
			A nsa;
			do {
				buffer_key skey(sfile, sblock++);
				s_inbuf = buf_manager.
						access_record_block<S,B,1>(skey);
				if ( NULL != s_inbuf )
					nsa = (*jas)( s_inbuf->records[0] );
			} while( NULL != s_inbuf && 0 == (*cmp)(sa, nsa) );
		} else { // S is greater
			// remove some values from R
			A nra;
			do {
				buffer_key skey(rfile, rblock++);
				r_inbuf = buf_manager.
						access_record_block<R,B,1>(skey);
				if ( NULL != r_inbuf )
					nra = (*jar)( r_inbuf->records[0] );
			} while( NULL != r_inbuf && 0 == (*cmp)(ra, nra) );
		}
	}
}



#endif /* PHYSICAL_OPERATORS_HPP_ */
