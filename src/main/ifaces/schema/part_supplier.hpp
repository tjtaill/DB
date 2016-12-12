/*
 * part_supplier.hpp
 *
 *  Created on: Nov 21, 2009
 *      Author: Troy
 */
#include "schema/record.hpp"
#include "schema/attribute_types.hpp"

#ifndef PART_SUPPLIER_HPP_
#define PART_SUPPLIER_HPP_
struct part_supplier : public record {
	int_type part_key;
	int_type supplier_key;
	int_type available_quantity;
	float_type supply_cost;
	comment_type comment;

	static const std::size_t disk_size = INT_DISK_SIZE(key) +
						INT_DISK_SIZE(supplier_key) +
						INT_DISK_SIZE(available_quantity) +
						FLOAT_DISK_SIZE(supply_cost) +
						sizeof(comment_type) +
						NEWLINE_DISK_SIZE -
						1; // remove extra chars for null terminators

	static part_supplier* deserialize( const char* const stream){
		std::size_t bytes_read = 0;
		part_supplier* s = new part_supplier();
		s->part_key = deserialize_int( stream, bytes_read );
		s->supplier_key = deserialize_int( stream, bytes_read );
		s->available_quantity =
				deserialize_int( stream, bytes_read );
		s->supply_cost = deserialize_float(stream, bytes_read);
		deserialize_char(stream, s->comment,
				bytes_read, sizeof(comment_type) - 1);
		return s;
	}

	static std::string serialize( const part_supplier* const s){
		std::stringstream ss;
		ss << serialize_int( s->part_key );
		ss << serialize_int( s->supplier_key );
		ss << serialize_int( s->available_quantity );
		ss << serialize_float( s->supply_cost );
		ss << serialize_char(s->comment,
			sizeof(comment_type) - 1 );
		ss << serialize_newline();
		return ss.str();
	}

};

#endif /* PART_SUPPLIER_HPP_ */
