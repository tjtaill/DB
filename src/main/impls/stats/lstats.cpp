/*
 * lstasts.cpp
 *
 *  Created on: Dec 3, 2009
 *      Author: ttaillef
 */

#include "stats/lstats.hpp"
#include "physical/physical_operators.hpp"
#include "schema/line_item.hpp"
#include "storage/record_block.hpp"
#include "schema/attribute_types.hpp"
#include <set>
#include <map>
#include <utility>
#include <string>
#include "storage/block.hpp"
#include <iostream>
#include "ini/ini_file.hpp"
#include <boost/lexical_cast.hpp>
#include <string>
#include <limits>


using namespace std;

static lstats stats;

static set<int_type> order_keys;
static set<int_type> part_keys;
static set<int_type> supplier_keys;
static int_type maxkey = numeric_limits<int_type>::min();




void lstats( record_block<line_item>* blk){
	line_item* c;
	if ( NULL == blk ) return;
	stats.nblocks++;
	for(size_t i = 0; i < block::nrecords; i++ ){
		c = blk->records[i];
		if ( NULL == c ) break;
		if ( c->order_key > maxkey ){
			maxkey = c->order_key;
		}
		stats.nrecords++;
		order_keys.insert( c->order_key );
		part_keys.insert( c->part_key );
		supplier_keys.insert( c->supplier_key );
	}

}


int main(int argc, char **argv) {
	using namespace boost;

	// read config file
	map<string,string> args = read_ini_file( argv[1] );

	string relation_path = args["relation.path"];

	string stats_path = args["stats.path"];


	table_scan<line_item, record_block, block::nrecords>(relation_path.c_str(), &lstats );
	stats.maxkey = maxkey;
	stats.norderkeys = order_keys.size();
	stats.npartkeys = part_keys.size();
	stats.nsupplierkeys = supplier_keys.size();

	map<string,string> ini_stats;


	ini_stats.insert( make_pair("line.item.records",
			lexical_cast<string>(stats.nrecords)));

	ini_stats.insert( make_pair("order.keys",
				lexical_cast<string>(stats.norderkeys)));

	ini_stats.insert( make_pair("part.keys",
					lexical_cast<string>(stats.npartkeys)));

	ini_stats.insert( make_pair("supplier.keys",
						lexical_cast<string>(stats.nsupplierkeys)));

	ini_stats.insert( make_pair("line.item.blocks",
							lexical_cast<string>(stats.nblocks)));

	ini_stats.insert( make_pair("max.key",
								lexical_cast<string>(stats.maxkey)));

	write_ini_file( ini_stats, stats_path);

	return 0;
}
