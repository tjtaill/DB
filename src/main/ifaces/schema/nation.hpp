/*
 * nation.hpp
 *
 *  Created on: Nov 21, 2009
 *      Author: Troy
 */
#include "record.hpp"
#include "attribute_types.hpp"

#ifndef NATION_HPP_
#define NATION_HPP_
struct nation : public record {
	int_type key;
	n_name_t name;
	int_type region_key;
	comment_type comment;

	static const std::size_t disk_size = INT_DISK_SIZE(key) +
						sizeof(n_name_t) +
						INT_DISK_SIZE(region_key) +
						sizeof(comment_type) +
						NEWLINE_DISK_SIZE -
						2; // remove extra chars for null terminators

	static nation* deserialize( const char* const stream){
		std::size_t bytes_read = 0;
		nation* n = new nation();
		n->key = deserialize_int( stream, bytes_read );
		deserialize_char(stream, n->name,
						bytes_read, sizeof(n_name_t) - 1);
		n->region_key = deserialize_int( stream, bytes_read );
		deserialize_char(stream, n->comment,
				bytes_read, sizeof(comment_type) - 1);
		return n;
	}

	static std::string serialize( const nation* const s){
		std::stringstream ss;
		ss << serialize_int( s->key );
		ss << serialize_char(s->name,
					sizeof(n_name_t) - 1 );
		ss << serialize_int( s->region_key );
		ss << serialize_char(s->comment,
			sizeof(comment_type) - 1 );
		ss << serialize_newline();
		return ss.str();
	}
};

#endif /* NATION_HPP_ */
