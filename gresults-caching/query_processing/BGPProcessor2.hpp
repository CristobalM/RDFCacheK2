//
// Created by cristobal on 7/11/21.
//

#ifndef RDFCACHEK2_BGPPROCESSOR2_HPP
#define RDFCACHEK2_BGPPROCESSOR2_HPP

#include "BGPOp.hpp"
#include "ResultTableIteratorBGP.hpp"
#include "Triple.hpp"
#include "VarIndexManager.hpp"
#include <PredicatesCacheManager.hpp>
#include <ResultTable.hpp>
#include <sparql_tree.pb.h>

#include <set>
#include <vector>

class BGPProcessor2 {

  const proto_msg::BGPNode &bgp_node;
  const PredicatesCacheManager &cm;
  VarIndexManager &vim;

  std::vector<Triple> triples;

  enum BAND_TYPE { COL_BAND = 0, ROW_BAND = 1 };

  std::set<unsigned long> header_values;
  std::vector<unsigned long> header_vec;

  std::vector<long> join_incidence;
  std::unordered_map<unsigned long, unsigned long> header_reverse_map;

public:
  BGPProcessor2(const proto_msg::BGPNode &bgp_node,
                const PredicatesCacheManager &cm, VarIndexManager &vim);

  std::shared_ptr<ResultTable> execute();
  std::shared_ptr<ResultTableIterator> execute_it();

private:
  void set_triples_from_proto();

  void find_headers();
  void add_variable(Term &term);
  std::vector<std::unique_ptr<K2TreeMixed::K2TreeScanner>> build_scanners();
  std::vector<std::unique_ptr<BGPOp>> build_bgp_ops(
      std::vector<std::unique_ptr<K2TreeMixed::K2TreeScanner>> &&scanners);
  void build_rev_map();
  bool do_all_predicates_have_trees();
};

#endif // RDFCACHEK2_BGPPROCESSOR2_HPP
