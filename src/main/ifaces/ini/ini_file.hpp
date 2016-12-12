/*
 * ini_file_reader.hpp
 *
 *  Created on: Nov 26, 2009
 *      Author: Troy
 */

#include <map>
#include <set>
#include <fstream>
#include <boost/config.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>
#include <utility>


#ifndef INI_FILE_READER_HPP_
#define INI_FILE_READER_HPP_
std::map<std::string, std::string> read_ini_file(
		std::string file_name ){
	std::map<std::string, std::string> arg_map;
	std::set<std::string> options;
	options.insert("*");

	std::ifstream file( file_name.c_str() );


	boost::program_options::detail::config_file_iterator
		it(file, options), end;

	for(; it != end; it++ ){
		arg_map[ it->string_key ] = it->value[0];
	}
	return arg_map;
}

void write_ini_file( std::map<std::string, std::string>& map,
		std::string file_name ){

	std::ofstream file( file_name.c_str() );

	typedef std::map<std::string, std::string>::iterator it_type;
	it_type it = map.begin();
	it_type end = map.end();

	for(; it != end; it++){
		file << it->first << " = " << it->second << "\n";
	}

}



#endif /* INI_FILE_READER_HPP_ */
