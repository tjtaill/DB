/*
 * buffer_key.cpp
 *
 *  Created on: Nov 28, 2009
 *      Author: Troy
 */
#ifdef VC
#include "stdafx.h"
#endif

#include "buffer/buffer_key.hpp"

bool operator==(buffer_key const& a, buffer_key const& b){
        return a.file_name == b.file_name &&
        		a.block_number == b.block_number;
};


std::size_t hash_value(buffer_key const& key){
        boost::hash<std::size_t> int_hasher;
        boost::hash<std::string> string_hasher;
        return int_hasher(key.block_number)
        		+ string_hasher(key.file_name);
};
