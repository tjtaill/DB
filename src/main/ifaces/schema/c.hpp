/*
 * c.hpp
 *
 *  Created on: Nov 27, 2009
 *      Author: Troy
 */
#include "schema/record.hpp"
#include "schema/attribute_types.hpp"

#ifndef C_HPP_
#define C_HPP_
struct c : record {
	float_type supplier_account_balance;
	s_name_t supplier_name;
	n_name_t nation_name;
	int_type part_key;
	p_mfgr_t part_mfgr;
	s_address_t supplier_address;
	s_phone_t supplier_phone_number;
	comment_type supplier_comment;

	static const std::size_t disk_size =
			FLOAT_DISK_SIZE(supplier_account_balance) +
			sizeof(s_name_t) +
			sizeof(n_name_t) +
			INT_DISK_SIZE(part_key) +
			sizeof(p_mfgr_t) +
			sizeof(s_address_t) +
			sizeof(s_phone_t) +
			sizeof(comment_type) +
			NEWLINE_DISK_SIZE -
			6; // remove extra chars for null terminators


		static std::string serialize(const c* const s) {
			std::stringstream ss;
			ss << serialize_float(s->supplier_account_balance);
			ss << serialize_char(s->supplier_name, sizeof(s_name_t) - 1);
			ss << serialize_char(s->nation_name, sizeof(n_name_t) - 1);
			ss << serialize_int(s->part_key);
			ss << serialize_char(s->part_mfgr, sizeof(p_mfgr_t) - 1);
			ss << serialize_char(s->supplier_address, sizeof(s_address_t) - 1);
			ss << serialize_char(s->supplier_phone_number,
					sizeof(s_phone_t) - 1);
			ss << serialize_char(s->supplier_comment, sizeof(comment_type) - 1);
			ss << serialize_newline();
			return ss.str();
		}




};

#endif /* C_HPP_ */
