/*
 * b.hpp
 *
 *  Created on: Nov 27, 2009
 *      Author: Troy
 */

#include "schema/record.hpp"
#include "schema/attribute_types.hpp"

#ifndef B_HPP_
#define B_HPP_
struct b : public record{
	b_countrycode_t country_code;
	float_type account_balance;

	static const std::size_t disk_size =
				sizeof(b_countrycode_t) +
				FLOAT_DISK_SIZE(supplier_account_balance) +
				NEWLINE_DISK_SIZE -
				1;

	static std::string serialize( const b* const r){
		std::stringstream ss;
		ss << serialize_char( r->country_code, sizeof(b_countrycode_t) - 1 );
		ss << serialize_float( r->account_balance );
		ss << serialize_newline();
		return ss.str();
	}

};

#endif /* B_HPP_ */
