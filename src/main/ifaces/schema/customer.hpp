/*
 * customer.hpp
 *
 *  Created on: Nov 21, 2009
 *      Author: Troy
 */
#include "record.hpp"
#include "attribute_types.hpp"
#include <cstddef>


#ifndef CUSTOMER_HPP_
#define CUSTOMER_HPP_
struct customer : public record {

	int_type key;
	c_name_t name;
	c_address_t address;
	int_type nation_key;
	c_phone_t phone_number;
	float_type account_balance;
	c_mktseg_t market_segment;
	comment_type comment;

	static const std::size_t disk_size = INT_DISK_SIZE(key) +
				sizeof(c_name_t) +
				sizeof(c_address_t) +
				INT_DISK_SIZE(nation_key) +
				sizeof(c_phone_t) +
				FLOAT_DISK_SIZE(supplier_account_balance) +
				sizeof(c_mktseg_t) +
				sizeof(comment_type) +
				NEWLINE_DISK_SIZE -
				5; // remove extra chars for null terminators

	static customer* deserialize( const char* const stream){
		std::size_t bytes_read = 0;
		customer* c = new customer();
		c->key = deserialize_int( stream, bytes_read );
		deserialize_char(stream, c->name, bytes_read, sizeof(c_name_t) - 1 );
		deserialize_char(stream, c->address, bytes_read, sizeof(c_address_t) - 1 );
		c->nation_key = deserialize_int(stream, bytes_read);
		deserialize_char(stream, c->phone_number, bytes_read, sizeof(c_phone_t) - 1);
		c->account_balance = deserialize_float(stream, bytes_read);
		deserialize_char(stream, c->market_segment, bytes_read, sizeof(c_mktseg_t) - 1);
		deserialize_char(stream, c->comment, bytes_read, sizeof(comment_type) - 1);
		return c;
	}

};

#endif /* CUSTOMER_HPP_ */
