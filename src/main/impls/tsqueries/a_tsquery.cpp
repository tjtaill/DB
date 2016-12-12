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
#include "schema/a.hpp"
#include "schema/line_item.hpp"
#include "storage/storage_manager.hpp"
#include "schema/attribute_types.hpp"
#include <string>
#include <map>
#include <iostream>

using namespace std;




date_type max_date, min_date;
a result;

void filter_date(record_block<line_item>* lb){
	for( size_t i = 0; i < block::nrecords; i++){
		line_item* lr = lb->records[i];
		if ( NULL == lr ) return;

		if( lr->receipt_date >= min_date &&
				lr->receipt_date <= max_date) {
			result.norders++;
			result.quantity_sum += lr->quantity;
			result.extending_price_sum += lr->extended_price;
			result.discount_price_sum += lr->extended_price *
				(1.0 - lr->discount);
			result.charge_sum = result.discount_price_sum;
		}
	}
}

int main(int argc, char **argv) {
	// read config file
	map<string, string> args = read_ini_file( argv[1] );

	string relation_path = args["relation.path"];

	cout << relation_path << endl;

	string results_path = args["results.path"];

	string fdb = args["first.date.boundary"];

	string sdb = args["second.date.boundary"];

	size_t b = 0;

	pad_date(fdb);
	pad_date(sdb);



	date_type fd = deserialize_ymd_date(fdb.c_str(), b);

	b = 0;
	date_type sd = deserialize_ymd_date(sdb.c_str(), b);


	max_date = fd > sd ? fd : sd;
	min_date = fd > sd ? sd : fd;



	table_scan(relation_path.c_str(), &filter_date);

	cout << blocks_ios << endl;

	result.average_quantity = result.quantity_sum / (float_type)result.norders;

	record_block<a> ab;
	ab.records[0] = &result;

	ofstream file(results_path.c_str());

	write_record_block(file, &ab);


	return 0;
}
