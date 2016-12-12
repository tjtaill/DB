/*
 * nsps_join.hpp
 *
 *  Created on: Dec 2, 2009
 *      Author: Troy
 */

#include "schema/record.hpp"
#include "schema/attribute_types.hpp"

#ifndef NSPS_JOIN_HPP_
#define NSPS_JOIN_HPP_
struct nsps_join: public record {
	int_type available_quantity;
	float_type supply_cost;
	int_type part_key;

	static const std::size_t disk_size =  INT_DISK_SIZE(available_quantity)	+
			FLOAT_DISK_SIZE(supply_cost) +
			INT_DISK_SIZE(part_key) +
			NEWLINE_DISK_SIZE; // remove extra chars for null terminators

	static nsps_join* deserialize(const char* const stream) {
		std::size_t bytes_read = 0;
		nsps_join* s = new nsps_join();
		s->available_quantity = deserialize_int(stream, bytes_read);
		s->supply_cost = deserialize_float(stream, bytes_read);
		s->part_key = deserialize_int(stream, bytes_read);
		return s;
	}

	static std::string serialize(const nsps_join* const s) {
		std::stringstream ss;
		ss << serialize_int(s->available_quantity);
		ss << serialize_float(s->supply_cost);
		ss << serialize_int(s->part_key);
		ss << serialize_newline();
		return ss.str();
	}
};

#endif /* NSPS_JOIN_HPP_ */
