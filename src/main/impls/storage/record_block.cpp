/*
 * record_block.cpp
 *
 *  Created on: Nov 24, 2009
 *      Author: Troy
 */
#ifdef VC
#include "stdafx.h"
#endif

#include "storage/record_block.hpp"
#include "buffer/buffer_manager.hpp"
#include "schema/customer.hpp"
#include "schema/a.hpp"
#include "schema/supplier.hpp"
#include "schema/b.hpp"
#include "schema/c.hpp"
#include "schema/line_item.hpp"
#include "schema/part_supplier.hpp"
#include "schema/nation.hpp"
#include "schema/sn_join.hpp"
#include "schema/region.hpp"
#include "schema/nr_join.hpp"
#include "schema/sps_join.hpp"
#include "schema/part.hpp"
#include "schema/pps_join.hpp"
#include "schema/d.hpp"
#include "schema/ns_join.hpp"
#include "schema/e.hpp"
#include "schema/nsps_join.hpp"

#include <cstddef>


template<typename R, size_t N>
record_block<R, N>::record_block(){
	for(std::size_t i = 0; i < N; i++){
			records[i] = NULL;
	}
}

template<typename R, size_t N>
record_block<R, N>::~record_block(){
	for(std::size_t i = 0; i < N; i++){
		if ( NULL != records[i] )
				delete records[i];
	}
}

template class record_block<nation>;
template class record_block<nation,1>;
template class record_block<nation,
	buffer_manager::max_memory_size / sizeof(nation)>;

template class record_block<nr_join>;
template class record_block<nr_join,1>;
template class record_block<nr_join,
	buffer_manager::max_memory_size / sizeof(nr_join)>;


template class record_block<sn_join>;
template class record_block<sn_join,1>;
template class record_block<sn_join,
	buffer_manager::max_memory_size / sizeof(sn_join)>;

template class record_block<ns_join>;
template class record_block<ns_join,1>;
template class record_block<ns_join,
	buffer_manager::max_memory_size / sizeof(ns_join)>;


template class record_block<sps_join>;
template class record_block<sps_join,1>;
template class record_block<sps_join,
	buffer_manager::max_memory_size / sizeof(sps_join)>;

template class record_block<nsps_join>;
template class record_block<nsps_join,1>;
template class record_block<nsps_join,
	buffer_manager::max_memory_size / sizeof(nsps_join)>;

template class record_block<pps_join>;
template class record_block<pps_join,1>;
template class record_block<pps_join,
	buffer_manager::max_memory_size / sizeof(pps_join)>;

template class record_block<region>;
template class record_block<region,1>;
template class record_block<region,
	buffer_manager::max_memory_size / sizeof(region)>;

template class record_block<supplier>;
template class record_block<supplier,1>;
template class record_block<supplier,
	buffer_manager::max_memory_size / sizeof(supplier)>;

template class record_block<part_supplier>;
template class record_block<part_supplier,1>;
template class record_block<part_supplier,
	buffer_manager::max_memory_size / sizeof(part_supplier)>;

template class record_block<part>;
template class record_block<part,1>;
template class record_block<part,
	buffer_manager::max_memory_size / sizeof(part)>;

template class record_block<customer>;
template class record_block<line_item>;
template class record_block<a>;
template class record_block<b>;
template class record_block<c>;
template class record_block<d>;
template class record_block<e>;
template class record_block<e,1>;
template class record_block<e, buffer_manager::max_memory_size / sizeof(e)>;

