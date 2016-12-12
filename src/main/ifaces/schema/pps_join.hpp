/*
 * pps_join.hpp
 *
 *  Created on: Nov 30, 2009
 *      Author: Troy
 */

#include "schema/record.hpp"
#include "schema/attribute_types.hpp"

#ifndef PPS_JOIN_HPP_
#define PPS_JOIN_HPP_
struct pps_join: public record {
	int_type supplier_key;
	s_name_t supplier_name;
	s_address_t supplier_address;
	s_phone_t supplier_phone_number;
	float_type supplier_account_balance;
	comment_type supplier_comment;
	n_name_t nation_name;
	int_type available_quantity;
	float_type supply_cost;
	int_type part_key;
	p_mfgr_t mfgr;

	static const std::size_t disk_size = INT_DISK_SIZE(supplier_key) +
		sizeof(s_name_t) + 
		sizeof(s_address_t) +
			sizeof(s_phone_t) +
			FLOAT_DISK_SIZE(supplier_account_balance) +
			sizeof(comment_type) +
			sizeof(n_name_t) +
			INT_DISK_SIZE(available_quantity) +
			FLOAT_DISK_SIZE(supply_cost) +
			INT_DISK_SIZE(part_key) +
			sizeof(p_mfgr_t) +
			NEWLINE_DISK_SIZE - 6; // remove extra chars for null terminators

	static pps_join* deserialize(const char* const stream) {
		std::size_t bytes_read = 0;
		pps_join* s = new pps_join();
		s->supplier_key = deserialize_int(stream, bytes_read);
		deserialize_char(stream, s->supplier_name, bytes_read,
				sizeof(s_name_t) - 1);
		deserialize_char(stream, s->supplier_address, bytes_read,
				sizeof(s_address_t) - 1);
		deserialize_char(stream, s->supplier_phone_number, bytes_read,
				sizeof(s_phone_t) - 1);
		s->supplier_account_balance = deserialize_float(stream, bytes_read);
		deserialize_char(stream, s->supplier_comment, bytes_read,
				sizeof(comment_type) - 1);
		deserialize_char(stream, s->nation_name, bytes_read,
				sizeof(n_name_t) - 1);
		s->available_quantity = deserialize_int(stream, bytes_read);
		s->supply_cost = deserialize_float(stream, bytes_read);
		s->part_key = deserialize_int(stream, bytes_read);
		deserialize_char(stream, s->mfgr, bytes_read,
						sizeof(p_mfgr_t) - 1);
		return s;
	}

	static std::string serialize(const pps_join* const s) {
		std::stringstream ss;
		ss << serialize_int(s->supplier_key);
		ss << serialize_char(s->supplier_name, sizeof(s_name_t) - 1);
		ss << serialize_char(s->supplier_address, sizeof(s_address_t) - 1);
		ss << serialize_char(s->supplier_phone_number,
				sizeof(s_phone_t) - 1);
		ss << serialize_float(s->supplier_account_balance);
		ss << serialize_char(s->supplier_comment, sizeof(comment_type) - 1);
		ss << serialize_char(s->nation_name, sizeof(n_name_t) - 1);
		ss << serialize_int(s->available_quantity);
		ss << serialize_float(s->supply_cost);
		ss << serialize_int(s->part_key);
		ss << serialize_char(s->mfgr, sizeof(p_mfgr_t) - 1);
		ss << serialize_newline();
		return ss.str();
	}
};

#endif /* PPS_JOIN_HPP_ */
