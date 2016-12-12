/*
 * index_operations.hpp
 *
 *  Created on: Dec 4, 2009
 *      Author: Troy
 */

#include "storage/index_block.hpp"
#include "schema/attribute_types.hpp"
#include "buffer/buffer_manager.hpp"
#include "buffer/buffer_key.hpp"
#include "storage/index_block.hpp"
#include "storage/storage_manager.hpp"
#include <cstddef>
#include <list>
#include <stack>
#include <set>
#include <string>
#include <boost/foreach.hpp>
#include <algorithm>
#include <limits>
#include <cassert>
#include <iostream>
#include <boost/foreach.hpp>

#ifndef INDEX_OPERATIONS_HPP_
#define INDEX_OPERATIONS_HPP_
static const int_type blk_offset = 1;
static int_type key_index = 0;

struct offset_blks: public std::unary_function<int_type&, void> {
	const int_type offset;

	offset_blks(int_type i) :
		offset(i) {

	}

	void operator()(int_type& i) {
		i = i == -1 ? i : i + offset;
	}
};

struct offset_blks_if: public std::unary_function<int_type&, void> {
	const std::set<int_type> change_blks;

	offset_blks_if(std::set<int_type> s) :
		change_blks(s) {

	}

	void operator()(int_type& i) {
		if (1 == change_blks.count(i))
			i++;
	}
};

template<std::size_t N>
int_type left_child_blkn(index_block<N>* ib, int_type key_index) {
	if (NULL == ib)
		return -1;

	int_type level = ib->level();
	int_type block_sum_this_level = 0;
	int_type block_sum_previous_level = 0;
	int_type level_block_number = 0;

	if (1 == level) {
		block_sum_this_level = 1;
		block_sum_previous_level = 0;
	} else if (2 == level) {
		block_sum_this_level = N + 1;
		block_sum_previous_level = 0;
	} else {
		block_sum_this_level = N + 1;
		block_sum_previous_level = N + 1;
		for (int i = 0; i < level - 2; i++) {
			block_sum_this_level *= N + 1;
		}
		for (int i = 0; i < level - 3; i++) {
			block_sum_previous_level *= N + 1;
		}
	}
	if (level >= 2) {
		block_sum_this_level++;
		block_sum_previous_level++;
	}

	level_block_number = ib->block_number - block_sum_previous_level;

	return block_sum_this_level + (N * level_block_number) + key_index;

}

template<std::size_t N>
int_type blks_lvl_above(int_type level) {

	int_type block_sum_previous_level = 0;

	if (1 == level) {
		block_sum_previous_level = 0;
	} else if (2 == level) {
		block_sum_previous_level = 1;
	} else {
		block_sum_previous_level = N + 1;
		for (int i = 0; i < level - 3; i++) {
			block_sum_previous_level *= N + 1;
		}
	}

	return block_sum_previous_level;
}

template<std::size_t N>
int_type level_blkn(index_block<N>* ib) {
	if (NULL == ib)
		return -1;

	int_type level = ib->level();
	int_type block_sum_previous_level = 0;

	if (1 == level) {
		block_sum_previous_level = 0;
	} else if (2 == level) {
		block_sum_previous_level = 0;
	} else {
		block_sum_previous_level = N + 1;
		for (int i = 0; i < level - 3; i++) {
			block_sum_previous_level *= N + 1;
		}
	}

	if (level >= 2) {
		block_sum_previous_level++;
	}

	return ib->block_number - block_sum_previous_level;
}

template<std::size_t N>
int_type level_blks(int_type leaf_level) {

	int_type level = leaf_level;
	int_type blks = 0;

	if (1 > level) {
		blks = 0;
	} else if (1 == level) {
		blks = 1;
	} else if (2 == level) {
		blks = N + 1;
	} else {
		blks = N + 1;
		for (int i = 0; i < level - 2; i++) {
			blks *= N + 1;
		}
	}

	return blks;
}

template<std::size_t N>
int_type all_blks_above(int_type level) {
	int_type sum = 0;

	for (int_type i = level - 1; i >= 1; i--) {
		sum += level_blks<N> (i);
	}
	return sum;
}

template<std::size_t N>
int_type level_blks(index_block<N>* ib) {
	if (NULL == ib)
		return -1;

	return level_blks<N> (ib->level());
}

template<std::size_t N>
void shift_index_file(const char* const index_file, int_type key,
		index_block<N>* start_blk, index_block<N>* parent_blk) {

	// copy the parent first it might get evicted from the cache
	index_block<N> pb(*parent_blk);
	int_type start_blkn = start_blk->block_number;

	std::cout << "shifting block " << start_blkn << " with parent "
			<< parent_blk->block_number << std::endl;
	// read whats in the disk slot now if it is unused do nothing
	buffer_key sk(index_file, start_blk->block_number);
	index_block<N>* blk = buf_manager.access_index_block<N> (sk);
	//std::cout << "looking at write location " << start_blkn << " found " << blk->block_number << std::endl;
	if (-1 == blk->block_number)
		return;

	int_type parent_blkn = parent_blk->block_number;

	int_type nodes_on_pl = level_blks<N> (parent_blk);

	int_type pl_blks = all_blks_above<N> (start_blk->level()) - blk_offset;

	// how many blocks are on this level
	int_type lvl_blks = level_blks<N> (start_blk);

	// this level end block
	int_type end_blkn = pl_blks + lvl_blks;

	// which block am I out of that
	int_type lvl_blkn = level_blkn<N> (start_blk);

	std::set<int_type> changed_blk_numbers;
	// check how many blocks we need to move

	std::cout << "looking for blocks to shift from " << start_blkn << " to "
			<< end_blkn - blk_offset << std::endl;
	int_type last_blkn = 0;
	for (int_type b = start_blkn; b < end_blkn - 1; b++) {
		buffer_key k(index_file, b);
		index_block<N>* blk = buf_manager.access_index_block<N> (k);
		if (-1 == blk->block_number)
			break;
		changed_blk_numbers.insert(blk->block_number);
		last_blkn = blk->block_number;
	}

	 std::cout << "change blk set " << std::endl;
	 BOOST_FOREACH(int_type e, changed_blk_numbers)
	 {	std::cout << e << ", ";
	 }
	 std::cout << std::endl;

	// modify the parent keys to the left of the key
	int_type* ip = std::lower_bound(&(pb.keys[0]), &(pb.keys[N]), key);
	int_type i = ip - &(pb.keys[0]);
	if (N - 1 > i) {
		std::for_each(&(pb.blocks[i]), &(pb.blocks[N + 1]), offset_blks_if(
				changed_blk_numbers));
		write_index_block<N> (index_file, &pb);
	}

	// so we need to add one to every block number we moved
	for (int_type b = parent_blkn + 1; b < nodes_on_pl; b++) {
		std::cout << "modding parent block " << b << std::endl;
		buffer_key k(index_file, b);
		index_block<N>* blk = buf_manager.access_index_block<N> (k);
		if (NULL == blk)
			break;
		if (-1 == blk->block_number || blk->is_leaf)
			continue;

		std::for_each(&(blk->blocks[0]), &(blk->blocks[N + 1]), offset_blks_if(
				changed_blk_numbers));

		write_index_block<N> (index_file, blk);
	}

	// we need to shift every node at the same level and to the right
	// of the start node one block down starting from the last to the
	// first
	std::cout << "shifting blocks right from " << last_blkn << " to "
			<< start_blkn << std::endl;
	for (int_type b = last_blkn; b >= start_blkn; b--) {
		buffer_key k(index_file, b);
		index_block<N>* blk = buf_manager.access_index_block<N> (k);
		if (NULL == blk)
			break;
		blk->block_number++;
		if (blk->is_leaf)
			blk->blocks[N]++;
		write_index_block<N> (index_file, blk);
	}

	//buf_manager.remove_buffers(); /// flush the cache its dirty
	// mod all blocks on current level too
	int_type start_lvl_blkn = all_blks_above<N> (start_blk->level());
	std::cout << "modding all blocks on parent level from blkn: "
			<< start_blkn << " to blkn: " << last_blkn << std::endl;

	for (int_type b = start_lvl_blkn; b < start_blkn; b++) {

		buffer_key k(index_file, b);
		index_block<N>* blk = buf_manager.access_index_block<N> (k);
		if (NULL == blk) {
			std::cout << b << "was null ?" << std::endl;
			break;
		}

		if (-1 == blk->block_number || blk->is_leaf )
			continue;

		std::cout << "modding sibling block " << b << std::endl;

		std::for_each(&(blk->blocks[0]), &(blk->blocks[N + 1]), offset_blks_if(
				changed_blk_numbers));

		write_index_block<N> (index_file, blk);
	}


	buf_manager.remove_buffers(); /// flush the cache its dirty
}

template<std::size_t N>
void increase_index_level(const char* const index_file, int leaf_level) {
	std::fstream file(index_file, std::ios::ate | std::ios::in | std::ios::out);

	std::size_t old_end = file.tellp();
	/*
	 std::cout << "increase tree level from: " << leaf_level << " to: "
	 << leaf_level + 1 << std::endl;

	 std::cout << "file pos: " << old_end << std::endl;

	 std::cout << "index disk size: " << index_block<N>::disk_size << std::endl;
	 */
	int nblk_offset = level_blks<N> (leaf_level + 1);

	//std::cout << "block offset : " << nblk_offset << std::endl;

	int previous_levels_blks = all_blks_above<N> (leaf_level + 1);
	// should there be a -1 or not ? Troy
	//std::cout << "previous level blks : " << previous_levels_blks << std::endl;

	std::size_t level_offset = nblk_offset * index_block<N>::disk_size;

	std::size_t p_offset = previous_levels_blks * index_block<N>::disk_size;

	//std::cout << " write offset : " << p_offset << std::endl;

	// append a bunch of default index blocks to grow the file
	/*
	 std::cout << " appending " << nblk_offset << " blocks to the file "
	 << std::endl;*/
	for (std::size_t i = 0; i < nblk_offset; i++) {
		index_block<N> ib;
		file << index_block<N>::serialize(&ib);
		blocks_ios++;
	}

	std::size_t new_end = file.tellp();

	file.close();

	file.open(index_file, std::ios::ate | std::ios::in | std::ios::out);

	// now shift all previous tree down except root down by N+1 blocks
	// update the block references for the non leaf nodes
	// so the point to the correct blocks
	// Troy you are here need to fix this.
	std::size_t rpos = p_offset - index_block<N>::disk_size;
	std::size_t wpos = p_offset + ((previous_levels_blks - 2)
			* index_block<N>::disk_size);
	std::size_t end = index_block<N>::disk_size;

	char buffer[index_block<N>::disk_size - 1];
	/*
	 std::cout << "copying blocks 1 to " << (p_offset
	 / index_block<N>::disk_size) - 1 << std::endl;
	 */
	int i = 0;
	while (rpos >= end) {

		//std::cout << "read seek to " << rpos << std::endl;
		file.seekg(rpos, std::ios::beg);

		//std::cout << " reading at : " << rpos << " till " << end << std::endl;
		file.getline(buffer, index_block<N>::disk_size);

		rpos -= index_block<N>::disk_size;

		index_block<N>* ib = index_block<N>::deserialize(buffer);

		assert( NULL != ib );

		if (-1 != ib->block_number) {
			std::cout << " changing blk to : " << ib->block_number << " to "
					<< ib->block_number + previous_levels_blks - blk_offset
					<< std::endl;
			ib->block_number += previous_levels_blks - blk_offset; // this has to be calculated based on level
		}

		if (!ib->is_leaf) {
			// have to update block numbers
			for (std::size_t i = 0; i < N + 1; i++) {
				if (-1 == ib->blocks[i])
					break;
				ib->blocks[i] += previous_levels_blks - blk_offset;
			}
		}

		//std::cout << "write seek to " << wpos << std::endl;
		file.seekp(wpos, std::ios::beg);
		/*
		 std::cout << " writing at : " << wpos << " till " << p_offset
		 << std::endl;
		 */
		file << index_block<N>::serialize(ib);

		wpos -= index_block<N>::disk_size;

		//		std::cout << " writing pos now at : " << wpos << std::endl;

		blocks_ios += 2;
		delete ib;
	}
	file.close();

}

template<std::size_t N>
std::set<int_type> search_key(std::string index_file, const int_type key) {
	std::set<int_type> blockns;
	// get the root node
	int_type iblockn = 0;
	std::cout << "searching for key " << key << std::endl;
	while (1) {
		std::size_t i = 0;
		buffer_key k(index_file, iblockn);

		index_block<N>* ib = buf_manager.access_index_block<N> (k);

		if (NULL == ib)
			return blockns;

		// if leaf node exact match
		// find the fisrt value that is equal or greater to the key
		int_type* ip = std::lower_bound(&(ib->keys[0]), &(ib->keys[N]), key);

		if (ib->is_leaf) {
			// we have to iterate over the keys to get all equal
			int_type* cp = ip;
			for (; cp < &(ib->keys[N]) && *cp == key; cp++) {
				i = cp - &(ib->keys[0]);
				std::cout << "key is in block " << ib->blocks[i] << std::endl;
				blockns.insert(ib->blocks[i]);
			}

			// no if we are not the root and the last key we continue to the
			// next leaf block if any
			if (0 == ib->block_number)
				return blockns;

			if ((*cp == key) && (N == i) && (-1 != ib->blocks[N])) { // we got to the end but still more keys
				std::cout << "continuing to block " << ib->blocks[N]
						<< std::endl;
				iblockn = ib->blocks[N];
				continue;
			}

			return blockns;

		} else { // continue to the next level
			i = ip - &(ib->keys[0]); // get index into the array
			if (key < *ip || N == i) {
				iblockn = ib->blocks[i];
			} else {
				iblockn = ib->blocks[i + 1];
			}
			std::cout << "choosing next block: " << iblockn << std::endl;
			continue;
		}
	}
	return blockns;
}

template<std::size_t N>
std::list<int_type> search_range(const char* const index_file,
		const int_type min_key, const int_type max_key) {
	std::list<int_type> blockns;
	return blockns;
}

template<std::size_t N>
void insert_nonfull_leaf(const char* const index_file, index_block<N>* ib,
		int_type key, int_type blkn) {

	std::cout << "inserting key: " << key << " into leaf blk "
			<< ib->block_number << std::endl;

	int_type* ip = std::lower_bound(&(ib->keys[0]), &(ib->keys[N]), key);
	std::size_t i = ip - &(ib->keys[0]);

	// std::cout << "shifting keys" << std::endl;
	if (N - 1 > i) {
		std::copy(ip, &(ib->keys[N - 1]), ip + 1);

		//memmove(ip + 1, ip, sizeof(int_type) * (&(ib->keys[N - 1]) - ip));

		std::copy(&(ib->blocks[i]), &(ib->blocks[N]), &(ib->blocks[i + 1]));
		/*
		 memmove(&(ib->blocks[i + 1]), &(ib->blocks[i]), sizeof(int_type)
		 * (&(ib->blocks[N]) - &(ib->blocks[i])));
		 * */
	} // else I am just putting it into last slot

	ib->keys[i] = key;
	ib->blocks[i] = blkn;
	if (0 != ib->block_number)
		ib->blocks[N] = ib->block_number + 1;
	ib->used++;

}

template<std::size_t N>
void insert_nonfull_parent(const char* const index_file, index_block<N>* ib,
		int_type key, int_type blkn) {
	int_type* ip = std::lower_bound(&(ib->keys[0]), &(ib->keys[N]), key);
	std::size_t i = ip - &(ib->keys[0]);

	std::cout << "insert median key: " << key << " into blk: "
			<< ib->block_number << std::endl;
	if (N - 1 > i) {
		std::copy(ip, &(ib->keys[N - 1]), ip + 1);
		//memmove(ip + 1, ip, sizeof(int_type) * (&(ib->keys[N - 1]) - ip));

		std::copy(&(ib->blocks[i]), &(ib->blocks[N]), &(ib->blocks[i + 1]));
		/*
		 memmove(&(ib->blocks[i + 1]), &(ib->blocks[i]), sizeof(int_type)
		 * (&(ib->blocks[N]) - &(ib->blocks[i])));
		 */
		//std::copy(&(ib->blocks[i]), &(ib->blocks[N]), &(ib->blocks[i + 1]));
	} // else I am just putting it into last slot

	ib->keys[i] = key;
	//ib->blocks[i] = blkn - 1;
	ib->is_leaf = false;
	ib->blocks[i + 1] = blkn;
	ib->used++;
}

template<std::size_t N>
int split_root(const char* const index_file, index_block<N>* ib, int_type key,
		int_type leaf_level) {
	assert( NULL != ib );
	assert( 0 == ib->block_number );
	// find the median
	std::size_t m = N / 2;
	int target_node = 0;
	increase_index_level<N> (index_file, leaf_level);
	// figure which node the key is going in to
	std::cout << "comparing median key " << key << " to root median key "
			<< ib->keys[m] << std::endl;
	if (key < ib->keys[m]) {
		target_node = 1;
	} else {
		target_node = 2;
	}
	//std::cout << "selecting target node as " << target_node << std::endl;

	if (!ib->is_leaf) {
		int_type plb = all_blks_above<N> (leaf_level + 1) - blk_offset;
		std::for_each(&(ib->blocks[0]), &(ib->blocks[N + 1]), offset_blks(plb));
	}

	// allocate three new index blocks
	index_block<N> p, left_child, right_child, empty;

	left_child.block_number = 1;
	std::copy(&(ib->keys[0]), &(ib->keys[m]), left_child.keys);
	std::copy(&(ib->blocks[0]), &(ib->blocks[m + 1]), left_child.blocks);
	left_child.used = m;
	left_child.blocks[N] = 2;

	right_child.block_number = 2;
	std::copy(&(ib->keys[m]), &(ib->keys[N]), right_child.keys);

	std::copy(&(ib->blocks[m]), &(ib->blocks[N + 1]), right_child.blocks);
	right_child.used = m + 1; // right child gets the median key
	right_child.blocks[N] = 3;

	if (!ib->is_leaf) {
		left_child.is_leaf = false;
		right_child.is_leaf = false;
	}

	p.block_number = 0;
	p.keys[0] = ib->keys[m];

	p.blocks[0] = 1;
	p.blocks[1] = 2;

	p.used = 1;
	p.is_leaf = false;

	// now update

	write_index_block<N> (index_file, &p);
	write_index_block<N> (index_file, &left_child);
	write_index_block<N> (index_file, &right_child);
	empty.is_leaf = false;
	write_index_blockn<N> (index_file, &empty, 3);
	write_index_blockn<N> (index_file, &empty, 4);

	buf_manager.remove_buffers();

	//if ( key_index == 8) exit(1);

	return target_node;
}

template<std::size_t N>
int split(const char* const index_file, index_block<N>* ib, int_type key,
		std::stack<int_type> parents, int_type leaf_level) {

	std::cout << "splitting block: " << ib->block_number << std::endl;

	if (parents.empty()) {
		return split_root<N> (index_file, ib, key, leaf_level);
	}

	// do right split

	int_type parent_blkn = parents.top();
	std::cout << "doing right split on block : " << ib->block_number
			<< " with parent: " << parent_blkn << std::endl;
	parents.pop();

	buffer_key pk(index_file, parent_blkn);
	index_block<N>* pib = buf_manager.access_index_block<N> (pk);

	assert(NULL != pib);

	int_type ib_blkn = ib->block_number;

	int root_split = 0;

	// calculate median
	std::size_t m = N / 2;

	if (N == pib->used) {
		// splitting the parent figure out where the key will go to the
		// right or left
		//int_type* ip = std::lower_bound(&(pib->keys[0]), &(pib->keys[N]), key);
		//int i = ip - &(pib->keys[0]);
		if (0 != pib->block_number) {
			/*
			 std::cout << "comparing median key " << ib->keys[m]
			 << " from child blk " << ib->block_number
			 << " with median key " << pib->keys[m]
			 << " from parent blk " << pib->block_number << std::endl;
			 */
			if (ib->keys[m] < pib->keys[m]) {
				// going in the original parent left side
			} else {
				// going in the new parent right side
				parent_blkn++;
			}
		}
		root_split = split(index_file, pib, ib->keys[m], parents, leaf_level);
	}

	// exit here see if the root split was done probably
	// if ( N == pib->used && ib->block_number == 4) exit(1);

	if (root_split) {

		int_type plb = all_blks_above<N> (leaf_level + 1) - blk_offset;
		ib_blkn += plb;
		if (0 != parent_blkn) {
			parent_blkn += plb;
		} else {
			std::cout << "root split changing parent block number to  "
					<< root_split << std::endl;
			parent_blkn = root_split;
		}
	}

	buffer_key k(index_file, ib_blkn);
	std::cout << "reloading block to split: " << ib_blkn << std::endl;
	ib = buf_manager.access_index_block<N> (k);

	buffer_key pk2(index_file, parent_blkn);
	std::cout << "reloading parent: " << parent_blkn << std::endl;
	pib = buf_manager.access_index_block<N> (pk2);

	assert( NULL != ib );
	assert( NULL != pib );

	index_block<N> rb;

	//std::cout << "copying keys to right child " << std::endl;
	// copy half left values into right child
	rb.block_number = ib_blkn + 1;
	std::copy(&(ib->keys[m]), &(ib->keys[N]), rb.keys);

	//std::cout << "copying blocks to right child " << std::endl;
	std::copy(&(ib->blocks[m]), &(ib->blocks[N + 1]), rb.blocks); // I thinks this was my mistake
	rb.used = m + 1; // right child gets the median key
	rb.is_leaf = ib->is_leaf;
	rb.blocks[N] = rb.block_number + 1;

	// modify left child values
	//std::cout << "modding left child " << std::endl;
	index_block<N> nl;
	nl.block_number = rb.block_number - 1;
	nl.is_leaf = rb.is_leaf;
	nl.blocks[N] = rb.block_number;
	std::copy(&(ib->blocks[0]), &(ib->blocks[m + 1]), nl.blocks);
	std::copy(&(ib->keys[0]), &(ib->keys[m]), nl.keys);
	nl.used = m;

	//std::cout << "writing children " << ib->block_number << " " << rb.block_number << std::endl;
	write_index_block(index_file, &nl);

	// shift to the right to make room for the right neighbo bloch
	//if ( key_index == 10 ) exit(1);
	shift_index_file(index_file, rb.keys[0], &rb, pib);

	write_index_block<N> (index_file, &rb);

	// reload parent block yet again
	//std::cout << "reloading parent " << std::endl;
	pib = buf_manager.access_index_block<N> (pk2);

	//std::cout << "modding parent " << std::endl;
	insert_nonfull_parent<N> (index_file, pib, rb.keys[0], rb.block_number);

	//std::cout << "writing parent " << std::endl;
	write_index_block<N> (index_file, pib);

	//std::cout << "purging cache " << std::endl;
	buf_manager.remove_buffers();
	//if ( key_index == 10 ) exit(1);
	return 0;
}

template<std::size_t N>
void insert_key(const char* const index_file, const int_type key,
		const int_type rblockn) {
	buf_manager.remove_buffers();

	assert( key != std::numeric_limits<int_type>::max() );
	key_index = key;
	int_type iblockn = 0;
	std::stack<int_type> parents;
	std::cout << "inserting key: " << key << std::endl;

	while (1) {
		std::size_t i = 0;
		buffer_key k(index_file, iblockn);
		//std::cout << "loading block: " << iblockn << std::endl;
		index_block<N>* ib = buf_manager.access_index_block<N> (k);

		if (NULL == ib) {
			// need to create a new node on the file

			index_block<N> iblk;
			iblk.block_number = iblockn;
			iblk.keys[0] = key;
			iblk.blocks[0] = rblockn;
			iblk.is_leaf = true;
			iblk.used = 1;
			write_index_block<N> (index_file, &iblk, true);
			return;
		}

		//std::string ss = index_block<N>::serialize(ib);
		//std::cout << ss << endl;

		if (ib->is_leaf) {
			// check if there is room
			if (ib->used < N) {
				// shift all values of right of new key over by one
				insert_nonfull_leaf<N> (index_file, ib, key, rblockn);
				// write out the record to disk
				write_index_block(index_file, ib);
				return;
			} else { // else we must split the node
				/*
				 std::cout << " about to split block " << ib->block_number << " at level "
				 << ib->level() << std::endl;
				 */
				split<N> (index_file, ib, key, parents, ib->level());
				/*
				 if (key == 7)
				 exit(1);
				 */
				// after we have slpit try to insert the key again;
				insert_key<N> (index_file, key, rblockn);
				return;
			}

		} else {// else we search done the tree for a leaf node
			parents.push(ib->block_number);
			int_type* ip =
					std::lower_bound(&(ib->keys[0]), &(ib->keys[N]), key);
			i = ip - &(ib->keys[0]);
			// std::cout << "descending the tree to index is: " << i << std::endl;
			if (key < *ip || N == i) {
				iblockn = ib->blocks[i];
			} else {
				iblockn = ib->blocks[i + 1];
			}
			std::cout << "descending the tree to blk : " << iblockn
					<< std::endl;
			continue;
		}
	}
}

#endif /* INDEX_OPERATIONS_HPP_ */
