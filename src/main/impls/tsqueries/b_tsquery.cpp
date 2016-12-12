/*
 * b_tsquery.cpp
 *
 *  Created on: Nov 27, 2009
 *      Author: Troy
 */
/*
 * a_tsquery.cpp
 *
 *  Created on: Nov 27, 2009
 *      Author: Troy
 */

#include "physical/physical_operators.hpp"
#include "ini/ini_file.hpp"
#include "storage/record_block.hpp"
#include "storage/block.hpp"
#include "schema/b.hpp"
#include "schema/customer.hpp"
#include "storage/storage_manager.hpp"
#include "schema/attribute_types.hpp"
#include <string>
#include <map>
#include <set>
#include <iostream>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>


using namespace std;
using namespace boost;

ofstream results_file;
set<string> country_codes;
float_type balance_sum = 0.0;
int_type nbalances = 0;
float_type avg_balance = 0.0;

void filter_sum(record_block<customer>* cb){


	for( size_t i = 0; i < block::nrecords; i++){
		customer* cr = cb->records[i];
		if ( NULL == cr ) return;

		string pn( cr->phone_number );
		string cc = pn.substr(0,2);

		if ( country_codes.count(cc) && cr->account_balance > 0.0){
			balance_sum += cr->account_balance;
			nbalances++;
		}
	}
}


void filter_write( record_block<customer>* cb ){
	record_block<b> br;
	int_type next = 0;

	for( size_t i = 0; i < block::nrecords; i++){
		customer* cr = cb->records[i];
		if ( NULL == cr ) break;

		string pn( cr->phone_number );
		string cc = pn.substr(0,2);

		if ( country_codes.count(cc) && cr->account_balance > avg_balance ){
			b* rb = new b();
			rb->account_balance = cr->account_balance;
			copy( cc.begin(), cc.end(), rb->country_code );
			rb->country_code[3] = '\0';
			br.records[next] = rb;
			next++;
		}
	}
	write_record_block(results_file, &br);
}


int main(int argc, char **argv) {
	// read config file
	map<string, string> args = read_ini_file( argv[1] );

	string relation_path = args["relation.path"];

	string results_path = args["results.path"];

	string codes = args["country.codes"];

	char_separator<char> sep(", ");
	tokenizer<char_separator<char> > tokens(codes, sep);
	BOOST_FOREACH(string t, tokens){
		country_codes.insert( t );
	}

	table_scan( relation_path.c_str(), &filter_sum);

	avg_balance = balance_sum / nbalances;

	results_file.open( results_path.c_str() );

	table_scan( relation_path.c_str(), &filter_write);

	cout << blocks_ios << endl;
	return 0;
}
