/*
 * supplier.hpp
 *
 *  Created on: Nov 21, 2009
 *      Author: Troy
 */
#include "schema/record.hpp"
#include "schema/attribute_types.hpp"
#include <string>
#include <sstream>

#ifndef SUPPLIER_HPP_
#define SUPPLIER_HPP_
struct supplier : public record {
	int_type key;
	s_name_t name;
	s_address_t address;
	int_type nation_key;
	s_phone_t phone_number;
	float_type account_balance;
	comment_type comment;


	static const std::size_t disk_size = INT_DISK_SIZE(key) +
		sizeof(s_name_t) +
					sizeof(s_address_t) +
					INT_DISK_SIZE(nation_key) +
					sizeof(s_phone_t) +
					FLOAT_DISK_SIZE(supplier_account_balance) +
					sizeof(comment_type) +
					NEWLINE_DISK_SIZE -
					4; // remove extra chars for null terminators

	static supplier* deserialize( const char* const stream){
			std::size_t bytes_read = 0;
			supplier* s = new supplier();
			s->key = deserialize_int( stream, bytes_read );
			deserialize_char(stream, s->name,
				bytes_read, sizeof(s_name_t) - 1 );
			deserialize_char(stream, s->address,
					bytes_read, sizeof(s_address_t) - 1 );
			s->nation_key = deserialize_int(stream, bytes_read);
			deserialize_char(stream, s->phone_number,
					bytes_read, sizeof(s_phone_t) - 1);
			s->account_balance = deserialize_float(stream,
					bytes_read);
			deserialize_char(stream, s->comment,
					bytes_read, sizeof(comment_type) - 1);
			return s;
	}

	static std::string serialize( const supplier* const s){
			std::stringstream ss;
			ss << serialize_int( s->key );
			ss << serialize_char( s->name,
				sizeof(s_name_t) - 1 );
			ss << serialize_char(
					s->address, sizeof(s_address_t) - 1 );
			ss << serialize_int( s->nation_key );
			ss << serialize_char(s->phone_number,
					sizeof(s_phone_t) - 1 );
			ss << serialize_float( s->account_balance );
			ss << serialize_char(s->comment,
								sizeof(comment_type) - 1 );
			ss << serialize_newline();
			return ss.str();
	}




};

#endif /* SUPPLIER_HPP_ */
