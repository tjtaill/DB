/*
 * a.hpp
 *
 *  Created on: Nov 26, 2009
 *      Author: Troy
 */

#include "record.hpp"
#include "attribute_types.hpp"
#include <string>
#include <sstream>


#ifndef A_HPP_
#define A_HPP_
struct a : public record{
	int_type quantity_sum;
	float_type extending_price_sum;
	float_type discount_price_sum;
	float_type charge_sum;
	float_type average_quantity;
	int_type norders;

	a(){
		quantity_sum = 0;
		extending_price_sum = 0.0;
		norders = 0;
		charge_sum = 0.0;
		discount_price_sum = 0.0;
		average_quantity = 0.0;
	}

	static const std::size_t disk_size =
			INT_DISK_SIZE(quantity_sum) +
			FLOAT_DISK_SIZE(extending_price_sum) +
			FLOAT_DISK_SIZE(discount_price_sum) +
			FLOAT_DISK_SIZE(charge_sum) +
			FLOAT_DISK_SIZE(average_quantity) +
			NEWLINE_DISK_SIZE;

	static std::string serialize( const a* const r){
		std::stringstream ss;
		ss << serialize_int( r->quantity_sum );
		ss << serialize_float( r->extending_price_sum );
		ss << serialize_float( r->discount_price_sum );
		ss << serialize_float( r->charge_sum );
		ss << serialize_float( r->average_quantity );
		ss << serialize_int( r->norders );
		ss << serialize_newline();
		return ss.str();
	}


};

#endif /* A_HPP_ */
