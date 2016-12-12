/*
 * es_tsquery.cpp
 *
 *  Created on: Dec 2, 2009
 *      Author: Troy
 */
#include "tsqueries/c_tsquery.hpp"
#include "tsqueries/e_tsquery.hpp"

using namespace std;



int main(int argc, char **argv) {
	map<string, string> args = read_ini_file(argv[1]);

	string part_supplier_relation = args["part.supplier.relation"];

	string nation_relation = args["nation.relation"];
	string supplier_relation = args["supplier.relation"];

	nation_name = args["nation.name"];

	results_path = args["results.path"];
	string ini_out = args["ini.out"];

	inialize_paths();

	// select nation
	boost::filesystem::path nsel(results_dir / boost::filesystem::path(
			"nsel.select"));

	filter_scan<nation, record_block, block::nrecords> (
			nation_relation.c_str(), nsel.string().c_str(), &nation_filter);

	// sort nation by nation key
	sort_scan<nation, record_block> (nsel.string().c_str(),
				nsort.string().c_str(), &n_nation_less);

	// sort supplier by nation key
	sort_scan<supplier, record_block> (supplier_relation.c_str(),
			ssort.string().c_str(), &s_nation_less);

	// sort join nation and supplier
	sort_join<int_type, nation, supplier, ns_join, record_block> (
			nsort.string().c_str(), ssort.string().c_str(),
			snjoin.string().c_str(), &nja, &sja, &int_cmp, &ns_joiner);

	// sort ns_join by supplier key
	sort_scan<ns_join, record_block> (snjoin.string().c_str(),
				snsort.string().c_str(),
				&ns_supplier_less);

	// sort part supplier by supplier key
	sort_scan<part_supplier, record_block>( part_supplier_relation.c_str(),
		pssort.string().c_str(),
		&ps_supplier_less);

	// join ns_join to part supplier
	sort_join<int_type, part_supplier, ns_join, nsps_join, record_block> (
				pssort.string().c_str(), snsort.string().c_str(),
				psjoin.string().c_str(),
				&ps_ja,
				&ns_ja,
				&int_cmp,
				&nsps_joiner);

	// now compute the inner query sum
	table_scan<nsps_join, record_block, block::nrecords>( psjoin.string().c_str(),
			&sum_products);

	product_sums *= 0.0001;

	args.insert( make_pair("product.value", boost::lexical_cast<string>( product_sums )) );


	write_ini_file( args, ini_out.c_str());

	return 0;
}
