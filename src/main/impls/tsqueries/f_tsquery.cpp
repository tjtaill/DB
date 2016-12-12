/*
 * f_tsquery.cpp
 *
 *  Created on: Dec 3, 2009
 *      Author: Troy
 */

#include "ini/ini_file.hpp"
#include <map>
#include <string>
#include "physical/physical_operators.hpp"
#include <boost/lexical_cast.hpp>
#include "schema/attribute_types.hpp"
#include "schema/line_item.hpp"
#include <cstddef>
#include <fstream>
#include <algorithm>
#include "storage/record_block.hpp"

using namespace boost;
using namespace std;

line_item item;
bool key_present = false;

void key_exists(record_block<line_item, block::nrecords>* blk) {
	if (key_present) return;

	if (NULL == blk)
		return;

	for (size_t i = 0; i < block::nrecords; i++) {
		if (NULL == blk->records[i])
			return;

		if (item.order_key == blk->records[i]->order_key && item.line_number
				== blk->records[i]->line_number) {
			key_present = true;
			return;
		}

	}

}

int main(int argc, char **argv) {
	// read ini file
	map<string, string> args = read_ini_file(argv[1]);

	string line_item_relation = args["line.item.relation"];
	string results_path = args["results.path"];

	item.order_key = lexical_cast<int_type> (args["order.key"]);

	item.line_number = lexical_cast<int_type> (args["line.number"]);

	// now scan to see if these two form a unique key not in the table
	table_scan<line_item, record_block, block::nrecords> (
			line_item_relation.c_str(), &key_exists);

	ofstream out(results_path.c_str());

	if (key_present) {
		out << "error key exists" << endl;
		return 0;
	}

	// else retrieve rest of values and append the file

	item.part_key = lexical_cast<int_type> (args["part.key"]);

	item.supplier_key = lexical_cast<int_type> (args["supplier.key"]);

	item.quantity = lexical_cast<int_type> (args["quantity"]);

	item.extended_price = lexical_cast<float_type> (args["extended.price"]);

	item.discount = lexical_cast<float_type> (args["discount"]);

	item.taxes = lexical_cast<float_type> (args["tax"]);

	string flag = args["return.flag"];

	copy(flag.begin(), flag.end(), item.return_flag);

	string status = args["line.status"];

	copy(status.begin(), status.end(), item.line_status);

	string sdate = args["ship.date"];

	pad_date(sdate);

	size_t b = 0;
	item.ship_date = deserialize_ymd_date(sdate.c_str(), b);

	string cdate = args["commit.date"];

	pad_date(cdate);

	b = 0;
	item.commit_date = deserialize_ymd_date(cdate.c_str(), b);

	string rdate = args["receipt.date"];

	pad_date(rdate);

	b = 0;
	item.receipt_date = deserialize_ymd_date(rdate.c_str(), b);

	string sinst = args["shipping.instructions"];

	copy(sinst.begin(), sinst.end(), item.ship_instructions);


	string smode = args["shipping.mode"];

	copy(smode.begin(), smode.end(), item.ship_mode);

	string com = args["comment"];

	copy(com.begin(), com.end(), item.comment);

	ofstream relation( line_item_relation.c_str(), ios::app);

	write_record<line_item>( relation, &item);

	out << "1 record affected" << endl;

	return 0;
}
