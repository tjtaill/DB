/*
 * nr_join.hpp
 *
 *  Created on: Nov 29, 2009
 *      Author: Troy
 */

#include "schema/record.hpp"
#include "schema/attribute_types.hpp"

#ifndef NR_JOIN_HPP_
#define NR_JOIN_HPP_
struct nr_join: public record {
	int_type nation_key;
	n_name_t nation_name;


	static const std::size_t disk_size = INT_DISK_SIZE(nation_key)
			+ sizeof(n_name_t)
			+ NEWLINE_DISK_SIZE - 1; // remove extra chars for null terminators

	static nr_join* deserialize(const char* const stream) {
		std::size_t bytes_read = 0;
		nr_join* n = new nr_join();
		n->nation_key = deserialize_int(stream, bytes_read);
		deserialize_char(stream, n->nation_name, bytes_read,
				sizeof(n_name_t) - 1);
		return n;
	}

	static std::string serialize(const nr_join* const s) {
		std::stringstream ss;
		ss << serialize_int(s->nation_key);
		ss << serialize_char(s->nation_name, sizeof(n_name_t) - 1);
		ss << serialize_newline();
		return ss.str();
	}
};

#endif /* NR_JOIN_HPP_ */
