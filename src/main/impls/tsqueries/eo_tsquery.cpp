/*
 * eo_tsquery.cpp
 *
 *  Created on: Dec 3, 2009
 *      Author: ttaillef
 */

#include "tsqueries/c_tsquery.hpp"
#include "tsqueries/e_tsquery.hpp"
#include "schema/e.hpp"
#include <cmath>

using namespace std;


float_type product_value = 0;

bool nsps_part_less(const nsps_join* s1, const nsps_join* s2) {
	if (NULL == s1)
		return false;
	if (NULL == s2)
		return true;
	return s1->part_key < s2->part_key;
}

bool e_value_greater(const e* s1, const e* s2) {
	if (NULL == s1)
		return false;
	if (NULL == s2)
		return true;
	return s1->value > s2->value;
}

bool nsps_part_equal(const nsps_join* s1, const nsps_join* s2) {
	if (NULL == s1 ||  NULL == s2)
		return false;
	return s1->part_key == s2->part_key;
}

void accum_products(const nsps_join* n, e* e){
	if ( NULL == n || NULL == e ) return;
	e->part_key = n->part_key;
	e->value += n->supply_cost * n->available_quantity;
}

bool value_filter(const e* n) {
	if (NULL == n)
		return false;
	return  n->value > product_value;
}


int main(int argc, char **argv) {
	map<string, string> args = read_ini_file(argv[1]);

	string part_supplier_relation = args["part.supplier.relation"];

	string nation_relation = args["nation.relation"];
	string supplier_relation = args["supplier.relation"];

	nation_name = args["nation.name"];

	results_path = args["results.path"];

	product_value =  boost::lexical_cast<float_type>(args["product.value"] );

	inialize_paths();

	// sort nsps_join on part key
	sort_scan<nsps_join, record_block> (psjoin.string().c_str(),
				pssort.string().c_str(), &nsps_part_less);

	// group nsps_join
	boost::filesystem::path nsps_group(results_dir / boost::filesystem::path(
				"nsps.group"));

	group_by<nsps_join, e, record_block>( pssort.string().c_str(),
			nsps_group.string().c_str(),
			&nsps_part_equal, &accum_products);


	// now filter the results
	boost::filesystem::path esel(results_dir / boost::filesystem::path(
					"e.select"));

	filter_scan<e, record_block, block::nrecords>(nsps_group.string().c_str(),
			esel.string().c_str(),
			&value_filter);

	// now sort the result by value
	sort_scan<e, record_block>(esel.string().c_str(),
			results_path.c_str(),
			&e_value_greater);



	return 0;
}
