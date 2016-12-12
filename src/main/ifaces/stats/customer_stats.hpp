/*
 * customer_stats.hpp
 *
 *  Created on: Nov 25, 2009
 *      Author: Troy
 */
#include "schema/attribute_types.hpp"


#ifndef CUSTOMER_STATS_HPP_
#define CUSTOMER_STATS_HPP_
struct customer_stats {
	int_type nrecords;
	int_type nkeys;
	int_type nbalances;
	int_type nphone_numbers;
};

#endif /* CUSTOMER_STATS_HPP_ */
