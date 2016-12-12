/*
 * part.hpp
 *
 *  Created on: Nov 21, 2009
 *      Author: Troy
 */
#include "schema/record.hpp"
#include "schema/attribute_types.hpp"

#ifndef PART_HPP_
#define PART_HPP_
struct part : public record {
	int_type key;
	p_name_t name;
	p_mfgr_t mfgr;
	p_brand_t brand;
	p_type_t type;
	int_type size;
	p_container_t container;
	float_type retail_price;
	comment_type comment;


	static const std::size_t disk_size = INT_DISK_SIZE(key) +
						sizeof(p_name_t) +
						sizeof(p_mfgr_t) +
						sizeof(p_brand_t) +
						sizeof(p_type_t) +
						INT_DISK_SIZE(size) +
						sizeof(p_container_t) +
						FLOAT_DISK_SIZE(retail_price) +
						sizeof(comment_type) +
						NEWLINE_DISK_SIZE -
						6; // remove extra chars for null terminators

	static part* deserialize( const char* const stream){
		std::size_t bytes_read = 0;
		part* n = new part();
		n->key = deserialize_int( stream, bytes_read );
		deserialize_char(stream, n->name,
						bytes_read, sizeof(p_name_t) - 1);
		deserialize_char(stream, n->mfgr,
								bytes_read, sizeof(p_mfgr_t) - 1);
		deserialize_char(stream, n->brand,
								bytes_read, sizeof(p_brand_t) - 1);
		deserialize_char(stream, n->type,
								bytes_read, sizeof(p_type_t) - 1);
		n->size = deserialize_int(stream, bytes_read);
		deserialize_char(stream, n->container,
								bytes_read, sizeof(p_container_t) - 1);
		n->retail_price = deserialize_float(stream, bytes_read);
		deserialize_char(stream, n->comment,
				bytes_read, sizeof(comment_type) - 1);
		return n;
	}

	static std::string serialize( const part* const s){
		std::stringstream ss;
		ss << serialize_int( s->key );
		ss << serialize_char(s->name,
					sizeof(p_name_t) - 1 );
		ss << serialize_char( s->mfgr, sizeof(p_mfgr_t) - 1);
		ss << serialize_char( s->brand, sizeof(p_brand_t) - 1);
		ss << serialize_char(s ->type, sizeof(p_type_t) - 1);
		ss << serialize_int(s->size);
		ss << serialize_char(s->container, sizeof(p_container_t) - 1);
		ss << serialize_float(s->retail_price );
		ss << serialize_char(s->comment,
			sizeof(comment_type) - 1 );
		ss << serialize_newline();
		return ss.str();
	}


};

#endif /* PART_HPP_ */
