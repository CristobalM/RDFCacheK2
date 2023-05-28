//
// Created by cristobal on 7/11/21.
//

#ifndef RDFCACHEK2_BGPPROCESSOR_HPP
#define RDFCACHEK2_BGPPROCESSOR_HPP

#include "BgpMessage.hpp"
#include "VarIndexManager.hpp"
#include "iterators/bgpops/BGPOp.hpp"
#include "manager/PredicatesCacheManager.hpp"

#include <TimeControl.hpp>
#include <query_processing/iterators/QueryIterator.hpp>
#include <set>
#include <vector>

namespace k2cache {
class BGPProcessor {

  BgpMessage bgp_message;
  PredicatesCacheManager &cm;
  const NodeIdsManager &node_ids_manager;
  VarIndexManager &vim;

  enum BAND_TYPE { COL_BAND = 0, ROW_BAND = 1 };

  std::set<unsigned long> header_values;
  std::vector<unsigned long> header_vec;

  std::vector<long> join_incidence;
  std::unordered_map<unsigned long, unsigned long> header_reverse_map;

  TimeControl &time_control;

public:
  BGPProcessor(BgpMessage bgp_message,
               PredicatesCacheManager &cm,
               const NodeIdsManager &node_ids_manager,
               VarIndexManager &vim,
               TimeControl &time_control);

  std::shared_ptr<QueryIterator> execute_it();

private:
  void find_headers();
  void add_variable(const BgpNode &node);
  std::vector<std::unique_ptr<K2TreeScanner>> build_scanners();
  std::vector<std::unique_ptr<BGPOp>>
  build_bgp_ops(std::vector<std::unique_ptr<K2TreeScanner>> &&scanners);
  void build_rev_map();
  bool do_all_predicates_have_trees();

  uint64_t get_cache_id(uint64_t real_id) const;
};

}

#endif // RDFCACHEK2_BGPPROCESSOR_HPP
