/*
 * c_tsquery.hpp
 *
 *  Created on: Dec 2, 2009
 *      Author: Troy
 */

#include "physical/physical_operators.hpp"
#include "ini/ini_file.hpp"
#include "storage/record_block.hpp"
#include "storage/block.hpp"
#include "schema/supplier.hpp"
#include "schema/part_supplier.hpp"
#include "schema/line_item.hpp"
#include "schema/nation.hpp"
#include "schema/sn_join.hpp"
#include "schema/nr_join.hpp"
#include "schema/region.hpp"
#include "storage/storage_manager.hpp"
#include "schema/attribute_types.hpp"
#include "schema/part_supplier.hpp"
#include "schema/sps_join.hpp"
#include "schema/pps_join.hpp"
#include "schema/part.hpp"
#include "schema/c.hpp"
#include "schema/attribute_types.hpp"
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>

#ifndef C_TSQUERY_HPP_
#define C_TSQUERY_HPP_

bool s_nation_less(const supplier* s1, const supplier* s2) {
	if (NULL == s1)
		return false;
	if (NULL == s2)
		return true;
	return s1->nation_key < s2->nation_key;
}

bool n_nation_less(const nation* s1, const nation* s2) {
	if (NULL == s1)
		return false;
	if (NULL == s2)
		return true;
	return s1->key < s2->key;
}

bool nr_nation_less(const nr_join* s1, const nr_join* s2) {
	if (NULL == s1)
		return false;
	if (NULL == s2)
		return true;
	return s1->nation_key < s2->nation_key;
}

bool sn_supplier_less(const sn_join* s1, const sn_join* s2) {
	if (NULL == s1)
		return false;
	if (NULL == s2)
		return true;
	return s1->supplier_key < s2->supplier_key;
}

bool ps_supplier_less(const part_supplier* s1, const part_supplier* s2) {
	if (NULL == s1)
		return false;
	if (NULL == s2)
		return true;
	return s1->supplier_key < s2->supplier_key;
}

bool region_less(const nation* s1, const nation* s2) {
	if (NULL == s1)
		return false;
	if (NULL == s2)
		return true;
	return s1->region_key < s2->region_key;
}

bool r_region_less(const region* s1, const region* s2) {
	if (NULL == s1)
		return false;
	if (NULL == s2)
		return true;
	return s1->key < s2->key;
}


bool part_less(const sps_join* s1, const sps_join* s2) {
	if (NULL == s1)
		return false;
	if (NULL == s2)
		return true;
	return s1->part_key < s2->part_key;
}


bool p_part_less(const part* s1, const part* s2) {
	if (NULL == s1)
		return false;
	if (NULL == s2)
		return true;
	return s1->key < s2->key;
}

int int_cmp(int_type a, int_type b) {
	if (a == b)
		return 0;
	if (a > b)
		return 1;
	return -1;
}

int pass_cmp(int_type a, int_type b) {
	return 0; // every value joins with every value
}



int_type nrja(const nation* n) {
	return n->region_key;
}

int_type rnja(const region* r) {
	return r->key;
}

nr_join* nr_joiner(const nation* n, const region* r) {
	if (NULL == n || NULL == r)
		return NULL;

	nr_join* nr = new nr_join();
	nr->nation_key = n->key;
	std::copy(&(n->name[0]), &(n->name[sizeof(n_name_t)]), nr->nation_name);
	return nr;
}

int_type sja(const supplier* s) {
	return s->nation_key;
}

int_type nr_join_ja(const nr_join* n) {
	return n->nation_key;
}

int_type ps_ja(const part_supplier* ps) {
	return ps->supplier_key;
}

int_type sn_join_a(const sn_join* sn) {
	return sn->supplier_key;
}

int_type sps_join_a(const sps_join* s) {
	return s->part_key;
}

int_type part_join_a(const part* p) {
	return p->key;
}

sps_join* sps_joiner(const part_supplier* ps, const sn_join* sn) {
	if (NULL == ps || NULL == sn)
		return NULL;
	sps_join* sps = new sps_join();
	sps->supplier_key = sn->supplier_key;
	std::copy(&(sn->supplier_address[0]),
			&(sn->supplier_address[sizeof(s_address_t)]), sps->supplier_address);
	std::copy(&(sn->supplier_phone_number[0]),
			&(sn->supplier_phone_number[sizeof(s_phone_t)]),
			sps->supplier_phone_number);
	sps->supplier_account_balance = sn->supplier_account_balance;
	std::copy(&(sn->supplier_comment[0]),
			&(sn->supplier_comment[sizeof(comment_type)]),
			sps->supplier_comment);
	std::copy(&(sn->nation_name[0]), &(sn->nation_name[sizeof(n_name_t)]),
			sps->nation_name);
	sps->available_quantity = ps->available_quantity;
	sps->supply_cost = ps->supply_cost;
	sps->part_key = ps->part_key;

	return sps;

}

pps_join* pps_joiner(const part* ps, const sps_join* sn) {
	if (NULL == ps || NULL == sn)
		return NULL;
	pps_join* sps = new pps_join();
	sps->supplier_key = sn->supplier_key;
	std::copy(&(sn->supplier_address[0]),
			&(sn->supplier_address[sizeof(s_address_t)]), sps->supplier_address);
	std::copy(&(sn->supplier_phone_number[0]),
			&(sn->supplier_phone_number[sizeof(s_phone_t)]),
			sps->supplier_phone_number);
	sps->supplier_account_balance = sn->supplier_account_balance;
	std::copy(&(sn->supplier_comment[0]),
			&(sn->supplier_comment[sizeof(comment_type)]),
			sps->supplier_comment);
	std::copy(&(sn->nation_name[0]), &(sn->nation_name[sizeof(n_name_t)]),
			sps->nation_name);
	sps->available_quantity = sn->available_quantity;
	sps->supply_cost = sn->supply_cost;
	sps->part_key = ps->key;
	std::copy(&(ps->mfgr[0]), &(ps->mfgr[sizeof(p_mfgr_t)]), sps->mfgr);
	return sps;

}
sn_join* sn_joiner(const supplier* s, const nr_join* n) {
	if (NULL == s || NULL == n)
		return NULL;
	sn_join* sn = new sn_join();
	sn->supplier_key = s->key;
	std::copy(&(s->address[0]), &(s->address[sizeof(s_address_t)]),
			sn->supplier_address);
	std::copy(&(s->phone_number[0]), &(s->phone_number[sizeof(s_phone_t)]),
			sn->supplier_phone_number);
	sn->supplier_account_balance = s->account_balance;
	std::copy(&(s->comment[0]), &(s->comment[sizeof(comment_type)]),
			sn->supplier_comment);
	std::copy(&(n->nation_name[0]), &(n->nation_name[sizeof(n_name_t)]),
			sn->nation_name);
	return sn;
}

float_type min_supply_cost = std::numeric_limits<float_type>::max();
int_type part_size = 0;
std::string region_name;

bool region_filter(const region* r) {
	if (NULL == r)
		return false;
	std::string rn(r->name);
	return rn == region_name;
}

bool filter_part(const part* r) {
	if (NULL == r)
		return false;
	return r->size == part_size;
}

void find_min(record_block<pps_join>* blk) {
	if (NULL == blk)
		return;
	for (std::size_t i = 0; i < block::nrecords; i++) {
		if (NULL == blk->records[i])
			return;
		if (blk->records[i]->supply_cost < min_supply_cost) {
			min_supply_cost = blk->records[i]->supply_cost;
		}
	}
}

bool ps_filter(const part_supplier* r) {
	if (NULL == r)
		return false;
	float_type diff = std::abs(r->supply_cost - min_supply_cost);
	return diff < 0.001;
}

std::ofstream out;

void project_c(record_block<pps_join>* blk) {
	if (NULL == blk)
		return;
	record_block<c> cblk;
	for (size_t i = 0; i < block::nrecords; i++) {
		if (NULL == blk->records[i])
			break;
		c* cr = new c();
		cr->supplier_account_balance
				= blk->records[i]->supplier_account_balance;
		std::copy(&(blk->records[i]->supplier_name[0]),
				&(blk->records[i]->supplier_name[sizeof(s_name_t)]),
				cr->supplier_name);
		std::copy(&(blk->records[i]->nation_name[0]),
				&(blk->records[i]->nation_name[sizeof(n_name_t)]),
				cr->nation_name);
		cr->part_key = blk->records[i]->part_key;
		std::copy(&(blk->records[i]->mfgr[0]),
				&(blk->records[i]->mfgr[sizeof(p_mfgr_t)]),
				cr->part_mfgr);
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
	write_record_block<c, record_block, block::nrecords> (out, &cblk);
}

std::string results_path;
boost::filesystem::path results;
boost::filesystem::path results_dir = results.parent_path();

boost::filesystem::path psort;
boost::filesystem::path psort2;
boost::filesystem::path nsort;
boost::filesystem::path rsort;
boost::filesystem::path	nrjoin;
boost::filesystem::path	nrsort;
boost::filesystem::path ssort;
boost::filesystem::path	snjoin;
boost::filesystem::path snsort;
boost::filesystem::path	pssort;
boost::filesystem::path psjoin;
boost::filesystem::path spssort;
boost::filesystem::path pspsjoin;


boost::filesystem::path	pssort2;
boost::filesystem::path psjoin2;
boost::filesystem::path spssort2;
boost::filesystem::path pspsjoin2;

void inialize_paths(){
	results = boost::filesystem::path(results_path);
	results_dir = boost::filesystem::path(results.parent_path());

	psort = boost::filesystem::path(results_dir / boost::filesystem::path(
					"p.sort"));
	psort = boost::filesystem::path(results_dir / boost::filesystem::path(
						"p2.sort"));
	rsort = boost::filesystem::path(results_dir / boost::filesystem::path(
				"r.sort"));
	nsort = boost::filesystem::path(results_dir / boost::filesystem::path(
			"nation.sort"));

	nrjoin = boost::filesystem::path(results_dir / boost::filesystem::path("nr.join"));
	nrsort = boost::filesystem::path(results_dir / boost::filesystem::path("nr.sort"));
	ssort = boost::filesystem::path(results_dir / boost::filesystem::path(
			"supplier.sort"));

	snjoin = boost::filesystem::path(results_dir / boost::filesystem::path("sn.join"));
	snsort = boost::filesystem::path(results_dir / boost::filesystem::path("sn.sort"));

	pssort = boost::filesystem::path(results_dir / boost::filesystem::path("ps.sort"));
	psjoin = boost::filesystem::path(results_dir / boost::filesystem::path("sps.join"));
	spssort = boost::filesystem::path(results_dir / boost::filesystem::path("sps.sort"));
	pspsjoin = boost::filesystem::path(results_dir / boost::filesystem::path("psps.join"));


	pssort2 = boost::filesystem::path(results_dir / boost::filesystem::path("ps2.sort"));
	psjoin2 = boost::filesystem::path(results_dir / boost::filesystem::path("sps2.join"));
	spssort2 = boost::filesystem::path(results_dir / boost::filesystem::path("sps2.sort"));
	pspsjoin2 = boost::filesystem::path(results_dir / boost::filesystem::path("psps2.join"));

}


#endif /* C_TSQUERY_HPP_ */
