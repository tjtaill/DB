/*
 * e.hpp
 *
 *  Created on: Dec 3, 2009
 *      Author: ttaillef
 */

#include "schema/record.hpp"
#include "schema/attribute_types.hpp"

#ifndef E_HPP_
#define E_HPP_
struct e: public record {
	int_type part_key;
	float_type value;

	e(){
		part_key = 0;
		value = 0;
	}


	static const std::size_t disk_size = INT_DISK_SIZE(part_key) +
			+ FLOAT_DISK_SIZE(value) +
			+ NEWLINE_DISK_SIZE;

	static e* deserialize(const char* const stream) {
		std::size_t bytes_read = 0;
		e* s = new e();
		s->part_key = deserialize_int(stream, bytes_read);
		s->value = deserialize_float(stream, bytes_read);
		return s;
	}

	static std::string serialize(const e* const s) {
		std::stringstream ss;
		ss << serialize_int(s->part_key);
		ss << serialize_float(s->value);
		ss << serialize_newline();
		return ss.str();
	}
};

#endif /* E_HPP_ */
