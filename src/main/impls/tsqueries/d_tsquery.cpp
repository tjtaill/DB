/*
 * ds_tsquery.cpp
 *
 *  Created on: Dec 2, 2009
 *      Author: Troy
 */

#include "tsqueries/c_tsquery.hpp"
#include "schema/d.hpp"

using namespace std;

string nation_name;



void project_d(record_block<sn_join, block::nrecords>* blk) {
	if (NULL == blk)
		return;
	record_block<d> cblk;
	for (size_t i = 0; i < block::nrecords; i++) {
		if (NULL == blk->records[i])
					break;
		d* cr = new d();
		cr->supplier_account_balance
				= blk->records[i]->supplier_account_balance;
		std::copy(&(blk->records[i]->supplier_name[0]),
				&(blk->records[i]->supplier_name[sizeof(s_name_t)]),
				cr->supplier_name);
		std::copy(&(blk->records[i]->nation_name[0]),
				&(blk->records[i]->nation_name[sizeof(n_name_t)]),
				cr->nation_name);
		std::copy(&(blk->records[i]->supplier_address[0]),
				&(blk->records[i]-> supplier_address[sizeof(s_address_t)]),
				cr->supplier_address);
		std::copy(&(blk->records[i]->supplier_phone_number[0]),
				&(blk->records[i]-> supplier_phone_number[sizeof(s_phone_t)]),
				cr->supplier_phone_number);
		std::copy(&(blk->records[i]->supplier_comment[0]),
				&(blk->records[i]-> supplier_comment[sizeof(comment_type)]),
				cr->supplier_comment);
		cblk.records[i] = cr;
	}
	write_record_block<d, record_block, block::nrecords> (out, &cblk);
}

bool sn_greater( const sn_join* sn1, const sn_join* sn2 ){
	if ( NULL == sn1 ) return false;
	if ( NULL == sn2 ) return true;
	string s1(sn1->nation_name);
	string s2(sn2->nation_name);
	return s2 < s1;
}

int main(int argc, char **argv) {
	map<string, string> args = read_ini_file(argv[1]);

	string supplier_relation = args["supplier.relation"];

	string nation_relation = args["nation.relation"];
	string region_relation = args["region.relation"];

	region_name = args["region.name"];

	results_path = args["results.path"];

	inialize_paths();

	// select region
	boost::filesystem::path rsel(results_dir / boost::filesystem::path(
			"rsel.select"));

	filter_scan<region, record_block, block::nrecords> (
			region_relation.c_str(), rsel.string().c_str(), &region_filter);

	// sort region on region key
	sort_scan<region, record_block> (rsel.string().c_str(),
				rsort.string().c_str(), &r_region_less);

	// sort nation on region key


	sort_scan<nation, record_block> (nation_relation.c_str(),
			nsort.string().c_str(), &region_less);

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
			snjoin.string().c_str(), &sja, &nr_join_ja, &int_cmp, &sn_joiner);

	// sort by sn_join by nation name
	sort_scan<sn_join, record_block>( snjoin.string().c_str() , snsort.string().c_str(),
			&sn_greater);

	// project the results
	out.open(results_path.c_str());
	table_scan<sn_join, record_block, block::nrecords> (
			snsort.string().c_str(), &project_d);
	return 0;
}
