/*
 * line_item.hpp
 *
 *  Created on: Nov 21, 2009
 *      Author: Troy
 */
#include "schema/record.hpp"
#include "schema/attribute_types.hpp"
#include <iostream>
#include <sstream>
#include <string>

#ifndef LINE_ITEM_HPP_
#define LINE_ITEM_HPP_
struct line_item : record {
	int_type order_key;
	int_type part_key;
	int_type supplier_key;
	int_type line_number;
	int_type quantity;
	float_type extended_price;
	float_type discount;
	float_type taxes;
	l_returnflag_t return_flag;
	l_linestatus_t line_status;
	date_type ship_date;
	date_type commit_date;
	date_type receipt_date;
	l_shipinstruct_t ship_instructions;
	l_shipmode_t ship_mode;
	comment_type comment;

	static const std::size_t disk_size = INT_DISK_SIZE(order_key) +
			INT_DISK_SIZE(part_key) +
			INT_DISK_SIZE(supplier_key) +
			INT_DISK_SIZE(line_number) +
			INT_DISK_SIZE(quantity) +
			FLOAT_DISK_SIZE(extended_price) +
			FLOAT_DISK_SIZE(discount) +
			FLOAT_DISK_SIZE(taxes) +
			sizeof(l_returnflag_t) +
			sizeof(l_linestatus_t) +
			DATE_DISK_SIZE(ship_date) +
			DATE_DISK_SIZE(commit_date) +
			DATE_DISK_SIZE(receipt_date) +
			sizeof(l_shipinstruct_t) +
			sizeof(l_shipmode_t) +
			sizeof(comment_type) +
			NEWLINE_DISK_SIZE -
			5; // remove extra chars for null terminators

	static line_item* deserialize( const char* const stream){
			std::size_t bytes_read = 0;
			line_item* r = new line_item();
			r->order_key = deserialize_int( stream, bytes_read );
			r->part_key = deserialize_int( stream, bytes_read );
			r->supplier_key = deserialize_int( stream, bytes_read );
			r->line_number = deserialize_int( stream, bytes_read );
			r->quantity = deserialize_int( stream, bytes_read );
			r->extended_price = deserialize_float(stream, bytes_read);
			r->discount = deserialize_float(stream, bytes_read);
			r->taxes = deserialize_float(stream, bytes_read);
			deserialize_char(stream, r->return_flag, bytes_read,
					sizeof(l_returnflag_t) - 1);
			deserialize_char(stream, r->line_status, bytes_read,
								sizeof(l_linestatus_t) - 1);
			r->ship_date = deserialize_ymd_date(stream, bytes_read);
			r->commit_date = deserialize_ymd_date(stream, bytes_read);
			r->receipt_date = deserialize_ymd_date(stream, bytes_read);
			deserialize_char(stream, r->ship_instructions, bytes_read,
				sizeof(l_shipinstruct_t) - 1);
			deserialize_char(stream, r->ship_mode, bytes_read,
				sizeof(l_shipmode_t) - 1);
			deserialize_char(stream, r->comment, bytes_read,
								sizeof(comment_type) - 1);
			return r;
	}

	static std::string serialize( const line_item* r ){
				std::size_t bytes_read = 0;
				std::stringstream ss;
				ss << serialize_int(r->order_key);
				ss << serialize_int(r->part_key );
				ss << serialize_int(r->supplier_key);
				ss << serialize_int(r->line_number);
				ss << serialize_int(r->quantity);
				ss << serialize_float(r->extended_price);
				ss << serialize_float(r->discount);
				ss << serialize_float(r->taxes);
				ss << serialize_char(r->return_flag, sizeof(l_returnflag_t) - 1);
				ss << serialize_char(r->line_status, sizeof(l_linestatus_t) - 1);
				ss << serialize_ymd_date( r->ship_date );
				ss << serialize_ymd_date(r->commit_date);
				ss << serialize_ymd_date(r->receipt_date);
				ss << serialize_char(r->ship_instructions, sizeof(l_shipinstruct_t) - 1);
				ss << serialize_char(r->ship_mode, sizeof(l_shipmode_t) - 1);
				ss << serialize_char(r->comment, sizeof(comment_type) - 1);
				ss << serialize_newline();
				return ss.str();
		}

};

#endif /* LINE_ITEM_HPP_ */
