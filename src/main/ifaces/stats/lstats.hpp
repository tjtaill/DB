/*
 * lstats.hpp
 *
 *  Created on: Dec 3, 2009
 *      Author: ttaillef
 */

#include "schema/attribute_types.hpp"

#ifndef LSTATS_HPP_
#define LSTATS_HPP_
struct lstats {
	int_type nrecords;
	int_type norderkeys;
	int_type npartkeys;
	int_type nsupplierkeys;
	int_type nblocks;
	int_type maxkey;

	lstats() {
		nrecords = 0;
		norderkeys = 0;
		npartkeys = 0;
		nsupplierkeys = 0;
		nblocks = 0;
		maxkey = 0;

	}
};

#endif /* LSTATS_HPP_ */
