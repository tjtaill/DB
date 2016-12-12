/*
 * co_tsquery.cpp
 *
 *  Created on: Dec 2, 2009
 *      Author: Troy
 */

#include "tsqueries/c_tsquery.hpp"

using namespace std;

int main(int argc, char **argv) {
	map<string, string> args = read_ini_file(argv[1]);

	string supplier_relation = args["supplier.relation"];

	string nation_relation = args["nation.relation"];
	string region_relation = args["region.relation"];

	string part_supplier_relation = args["part.supplier.relation"];
	string part_relation = args["part.relation"];

	min_supply_cost = boost::lexical_cast<float_type>(args["min.supply.cost"]);

	region_name = args["region.name"];
	part_size = boost::lexical_cast<int_type>(args["part.size"]);

	results_path = args["results.path"];

	inialize_paths();

	// start outer query
	// do selections

	// select part
	boost::filesystem::path psel(results_dir / boost::filesystem::path(
			"ps.select"));

	filter_scan<part, record_block, block::nrecords> (part_relation.c_str(),
			psel.string().c_str(), &filter_part);

	// select part supplier
	boost::filesystem::path pssel(results_dir / boost::filesystem::path(
			"p.select"));

	filter_scan<part_supplier, record_block, block::nrecords> (
			part_supplier_relation.c_str(), pssel.string().c_str(), &ps_filter);

	// sort ps on supp key
	sort_scan<part_supplier, record_block>( pssel.string().c_str(),
					pssort2.string().c_str(),
					&ps_supplier_less);

	// join part supplier to sn_join
	sort_join<int_type, part_supplier, sn_join, sps_join, record_block> (
			pssort2.string().c_str(), snsort.string().c_str(),
			psjoin2.string().c_str(), &ps_ja, &sn_join_a, &int_cmp, &sps_joiner);

	// sort sps_join on part key
	sort_scan<sps_join, record_block> (psjoin2.string().c_str(),
			spssort2.string().c_str(), &part_less);

	// sort part on part key
	sort_scan<part, record_block> (psel.string().c_str(),
					psort2.string().c_str(), &p_part_less);

	sort_join<int_type, part, sps_join, pps_join, record_block> (
			psort2.string().c_str(), spssort2.string().c_str(),
			pspsjoin2.string().c_str(), &part_join_a, &sps_join_a, &int_cmp,
			&pps_joiner);

	// now project out desired values
	out.open(results_path.c_str());
	table_scan<pps_join, record_block, block::nrecords> (pspsjoin2.string().c_str(),
			&project_c);
	return 0;
}
