/*
 * ns_join.hpp
 *
 *  Created on: Dec 2, 2009
 *      Author: Troy
 */

#include "schema/record.hpp"
#include "schema/attribute_types.hpp"

#ifndef NS_JOIN_HPP_
#define NS_JOIN_HPP_
struct ns_join : public record {
	int_type supplier_key;

	static const std::size_t disk_size = INT_DISK_SIZE(supplier_key)
			+ NEWLINE_DISK_SIZE; // remove extra chars for null terminators

	static ns_join* deserialize(const char* const stream) {
		std::size_t bytes_read = 0;
		ns_join* s = new ns_join();
		s->supplier_key = deserialize_int(stream, bytes_read);
		return s;
	}

	static std::string serialize(const ns_join* const s) {
		std::stringstream ss;
		ss << serialize_int(s->supplier_key);
		ss << serialize_newline();
		return ss.str();
	}

};

#endif /* NS_JOIN_HPP_ */
