/*
 * region.hpp
 *
 *  Created on: Nov 21, 2009
 *      Author: Troy
 */
#include "schema/record.hpp"
#include "schema/attribute_types.hpp"

#ifndef REGION_HPP_
#define REGION_HPP_
struct region: public record {
	int_type key;
	r_name_t name;
	comment_type comment;

	static const std::size_t disk_size =
			INT_DISK_SIZE(key) +
			sizeof(r_name_t) +
			sizeof(comment_type) +
			NEWLINE_DISK_SIZE
			- 2; // remove extra chars for null terminators

	static region* deserialize(const char* const stream) {
		std::size_t bytes_read = 0;
		region* r = new region();
		r->key = deserialize_int(stream, bytes_read);
		deserialize_char(stream, r->name,
				bytes_read, sizeof(r_name_t) - 1);
		deserialize_char(stream, r->comment,
				bytes_read, sizeof(comment_type) - 1);
		return r;
	}

	static std::string serialize(const region* const r) {
		std::stringstream ss;
		ss << serialize_int(r->key);
		ss << serialize_char(r->name, sizeof(r_name_t) - 1);
		ss << serialize_char(r->comment, sizeof(comment_type) - 1);
		ss << serialize_newline();
		return ss.str();
	}
};

#endif /* REGION_HPP_ */
