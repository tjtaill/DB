/*
 * index_block.cpp
 *
 *  Created on: Dec 4, 2009
 *      Author: ttaillef
 */

#ifdef VC
#include "stdafx.h"
#endif

#include "storage/index_block.hpp"
#include <cstddef>


template class index_block<INDEX_N>;
template class index_block<3>; // for testing

template <std::size_t N>
std::size_t index_block<N>::memory_size(){
	return sizeof(index_block<N>);
}

template <std::size_t N>
index_block<N>::~index_block(){

}
