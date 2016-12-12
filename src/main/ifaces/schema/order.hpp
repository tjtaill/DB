/*
 * order.hpp
 *
 *  Created on: Nov 21, 2009
 *      Author: Troy
 */
#include "schema/record.hpp"
#include "schema/attribute_types.hpp"

#ifndef ORDER_HPP_
#define ORDER_HPP_
struct order : public record {
	int_type key;
	int_type customer_key;
	o_orderstatus_t order_status;
	float_type total_price;
	date_type date;
	o_oderpriority_t order_priority;
	o_orderclerk_t clerk[21];
	int_type ship_priority;
	comment_type comment;

};

#endif /* ORDER_HPP_ */
