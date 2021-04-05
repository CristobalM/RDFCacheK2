
//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_BGPPROCESSOR_HPP
#define RDFCACHEK2_BGPPROCESSOR_HPP

#include <memory>
#include <request_msg.pb.h>

#include "GroupedTriples.hpp"
#include "PredicatesCacheManager.hpp"
#include "ResultTable.hpp"
#include "VarIndexManager.hpp"

class BGPProcessor {

  const proto_msg::BGPNode &bgp_node;
  const PredicatesCacheManager &cm;
  VarIndexManager &vim;

public:
  BGPProcessor(const proto_msg::BGPNode &bgp_node,
               const PredicatesCacheManager &cm, VarIndexManager &vim);

  std::shared_ptr<ResultTable> execute();

private:
  GroupedTriples group_triple_nodes();
  std::shared_ptr<ResultTable>
  join_single_var_group(const std::string &var_name,
                        const std::vector<Triple> &triples);

  std::shared_ptr<ResultTable>
  join_two_tables_with_trees(std::shared_ptr<ResultTable> &table_one,
                             std::shared_ptr<ResultTable> &table_two,
                             KeyPairStr &keypair, std::vector<Triple> &triples);

  std::shared_ptr<ResultTable>
  join_table_with_trees_by_two_var(std::shared_ptr<ResultTable> &table,
                                   KeyPairStr &keypair,
                                   std::vector<Triple> &triples);

  std::shared_ptr<ResultTable> join_table_with_trees_by_one_var(
      std::shared_ptr<ResultTable> &table, const std::string &table_var,
      const std::string &other_var, std::vector<Triple> &triples);

  std::shared_ptr<ResultTable> join_table_with_trees_by_one_var_sip(
      std::shared_ptr<ResultTable> &table, const std::string &table_var,
      const std::string &other_var, std::vector<Triple> &triples);

  std::shared_ptr<ResultTable>
  join_two_var_group(const std::string &var_one, const std::string &var_two,
                     const std::vector<Triple> &triples);

  std::shared_ptr<ResultTable> cross_product_partial_results(
      std::unordered_map<std::string, std::shared_ptr<ResultTable>>
          &partial_results);

  std::shared_ptr<ResultTable> join_two_var_group_merge(
      const std::string &var_one, const std::string &var_two,
      const std::vector<Triple> &triples,
      std::unordered_map<std::string, K2TreeMixed *> &k2trees_map);

  std::shared_ptr<ResultTable>
  join_two_var_group_sip(const std::string &var_one, const std::string &var_two,
                         const std::vector<Triple> &triples);

  unsigned long get_index_from_term(const Term &term);
};

#endif
