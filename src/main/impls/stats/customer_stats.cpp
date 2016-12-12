/*
 * customer_stats.cpp
 *
 *  Created on: Nov 25, 2009
 *      Author: Troy
 */

#include "stats/customer_stats.hpp"
#include "physical/physical_operators.hpp"
#include "schema/customer.hpp"
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


using namespace std;

static customer_stats cstats;

static set<int_type> keys;
static set<float_type> balances;
static set<string> pnumbers;



void cstat( record_block<customer>* cb){
	customer* c;
	if ( NULL == cb ) return;
	for(size_t i = 0; i < block::nrecords; i++ ){
		c = cb->records[i];
		if ( NULL == c ) break;
		cstats.nrecords++;
		keys.insert( c->key );
		balances.insert( c->account_balance );
		pnumbers.insert( c->phone_number );
	}
}


int main(int argc, char **argv) {
	using namespace boost;

	// read config file
	map<string,string> args = read_ini_file( argv[1] );

	string relation_path = args["relation.path"];

	string stats_path = args["stats.path"];


	table_scan(relation_path.c_str(), &cstat );

	cstats.nkeys = keys.size();
	cstats.nbalances = balances.size();
	cstats.nphone_numbers = pnumbers.size();

	map<string,string> stats;


	stats.insert( make_pair("customer.records",
			lexical_cast<string>(cstats.nrecords)));

	stats.insert( make_pair("customer.keys",
				lexical_cast<string>(cstats.nkeys)));

	stats.insert( make_pair("customer.balances",
					lexical_cast<string>(cstats.nbalances)));

	stats.insert( make_pair("customer.numbers",
						lexical_cast<string>(cstats.nphone_numbers)));

	write_ini_file( stats, stats_path);

	return 0;
}
