/*
 * e_tsquery.hpp
 *
 *  Created on: Dec 2, 2009
 *      Author: Troy
 */

#include "schema/supplier.hpp"
#include "schema/ns_join.hpp"
#include "schema/nation.hpp"
#include "schema/nsps_join.hpp"

#ifndef E_TSQUERY_HPP_
#define E_TSQUERY_HPP_
std::string nation_name;

bool nation_filter(const nation* n) {
	if (NULL == n)
		return false;
	std::string nn(n->name);
	return nn == nation_name;
}

int_type nja(const nation* n) {
	return n->key;
}

ns_join* ns_joiner(const nation* n, const supplier* r) {
	if (NULL == n || NULL == r)
		return NULL;

	ns_join* nr = new ns_join();
	nr->supplier_key = r->key;
	return nr;
}

nsps_join* nsps_joiner(const part_supplier* n, const ns_join* r) {
	if (NULL == n || NULL == r)
		return NULL;

	nsps_join* nr = new nsps_join();
	nr->available_quantity = n->available_quantity;
	nr->part_key = n->part_key;
	nr->supply_cost = n->supply_cost;
	return nr;
}

bool ns_supplier_less(const ns_join* s1, const ns_join* s2) {
	if (NULL == s1)
		return false;
	if (NULL == s2)
		return true;
	return s1->supplier_key < s2->supplier_key;
}

int_type nsps_ja(const nsps_join* ps) {
	return ps->part_key;
}

int_type ns_ja(const ns_join* ps) {
	return ps->supplier_key;
}

float_type product_sums = 0;

void sum_products(record_block<nsps_join>* blk) {
	if (NULL == blk)
		return;
	for (std::size_t i = 0; i < block::nrecords; i++) {
		if (NULL == blk->records[i])
			return;

		product_sums += blk->records[i]->supply_cost * blk->records[i]->available_quantity;
	}
}
#endif /* E_TSQUERY_HPP_ */
