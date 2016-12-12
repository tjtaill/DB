/*
 * c_tsquery.cpp
 *
 *  Created on: Nov 28, 2009
 *      Author: Troy
 */

#ifdef VC
#include "stdafx.h"
#endif

#include "tsqueries/c_tsquery.hpp"
#include <utility>

using namespace std;

int main(int argc, char **argv) {
	// read config file
	map<string, string> args = read_ini_file(argv[1]);
	string supplier_relation = args["supplier.relation"];

	string nation_relation = args["nation.relation"];
	string region_relation = args["region.relation"];

	string part_supplier_relation = args["part.supplier.relation"];
	string part_relation = args["part.relation"];

	region_name = args["region.name"];

	string ini_out = args["ini.out"];

	results_path = args["results.path"];

	inialize_paths();

	// start subquery



	// select region
	boost::filesystem::path rsel(results_dir / boost::filesystem::path(
								"rsel.select"));




	filter_scan<region, record_block, block::nrecords>( region_relation.c_str(),
			rsel.string().c_str(),
					&region_filter);

	// sort region on region key
	sort_scan<region, record_block>(rsel.string().c_str(),
				rsort.string().c_str(),
				&r_region_less);

	// sort nation on region key


	sort_scan<nation, record_block>(nation_relation.c_str(),
			nsort.string().c_str(),
			&region_less);

	// join nation and region


	sort_join<int_type, nation, region, nr_join, record_block> (
			nsort.string().c_str(), rsort.string().c_str(),
			nrjoin.string().c_str(), &nrja, &rnja, &int_cmp, &nr_joiner);

	// sort nr by nation key
	sort_scan<nr_join, record_block> (nrjoin.string().c_str(),
				nrsort.string().c_str(), &nr_nation_less);


	// sort supplier by nation key
	sort_scan<supplier, record_block> (supplier_relation.c_str(),
			ssort.string().c_str(), &s_nation_less);


	// sort join nr_join and supplier
	sort_join<int_type, supplier, nr_join, sn_join, record_block> (
			ssort.string().c_str(), nrsort.string().c_str(),
			snjoin.string().c_str(), &sja, &nr_join_ja,
			&int_cmp, &sn_joiner);

	

	// sort sn_join on supplier key
	sort_scan<sn_join, record_block>( snjoin.string().c_str(),
			snsort.string().c_str(),
			&sn_supplier_less);

	// now sort part_supplier on supplier key

// here is different then outer query
	sort_scan<part_supplier, record_block>( part_supplier_relation.c_str(),
				pssort.string().c_str(),
				&ps_supplier_less);


	// now sort join part_supplier and sn_join


	sort_join<int_type, part_supplier, sn_join, sps_join, record_block> (
		pssort.string().c_str(), snsort.string().c_str(),
		psjoin.string().c_str(), &ps_ja, &sn_join_a,
		&int_cmp, &sps_joiner);


	// now sort sps by part_key


	sort_scan<sps_join, record_block>( psjoin.string().c_str(),
			spssort.string().c_str(),
			&part_less);


	// sort part by by part key
	sort_scan<part, record_block>(part_relation.c_str(),
				psort.string().c_str(),
				&p_part_less);


	// now join part with sps_join



	sort_join<int_type, part, sps_join, pps_join, record_block>(psort.string().c_str(),
			spssort.string().c_str(),
			pspsjoin.string().c_str(),
			&part_join_a, &sps_join_a, &int_cmp, &pps_joiner);

	// find minimum supply cost
	table_scan<pps_join, record_block>( pspsjoin.string().c_str(),
			&find_min);

	args.insert( make_pair("min.supply.cost", boost::lexical_cast<string>( min_supply_cost)) );


	write_ini_file( args, ini_out.c_str());
	// end subquery


	return 0;
}
