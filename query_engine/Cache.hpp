//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHE_HPP
#define RDFCACHEK2_CACHE_HPP

#include <memory>
#include <mutex>
#include <string>

#include <PredicatesCacheManager.hpp>
#include <TimeControl.hpp>
#include <caching/I_CacheReplacement.hpp>
#include <query_processing/ParsingUtils.hpp>
#include <query_processing/QueryResultIteratorHolder.hpp>
#include <request_msg.pb.h>

#include "query_processing/QueryResult.hpp"

struct CacheStats {
  int allocated_u32s;
  int nodes_count;
  int containers_sz_sum;
  int frontier_data;
  int blocks_data;

  int max_points_k2;
  int number_of_points_avg;
  int blocks_counted;
};

class Cache {
  std::shared_ptr<PredicatesCacheManager> cache_manager;

  std::string temp_files_dir;
  unsigned long timeout_ms;

  std::unique_ptr<I_CacheReplacement> cache_replacement;

  I_CacheReplacement::REPLACEMENT_STRATEGY strategy_id;

public:
  using cm_t = std::shared_ptr<PredicatesCacheManager>;
  using pcm_t = PredicatesCacheManager;
  Cache(std::shared_ptr<PredicatesCacheManager> &cache_manager,
        size_t memory_budget_bytes, std::string temp_files_dir,
        unsigned long timeout_ms,
        I_CacheReplacement::REPLACEMENT_STRATEGY replacement_strategy);

  std::shared_ptr<QueryResultIteratorHolder>
  run_query(const proto_msg::SparqlTree &query_tree, TimeControl &time_control);
  RDFResource extract_resource(unsigned long index) const;

  PredicatesCacheManager &get_pcm();
  bool query_is_valid(const proto_msg::SparqlTree &tree);
  bool node_is_valid(const proto_msg::SparqlNode &node);
  bool bgp_node_is_valid(const proto_msg::BGPNode &node);
  bool union_node_is_valid(const proto_msg::UnionNode &node);
  bool sequence_is_valid(const proto_msg::SequenceNode &node);
  void ensure_available_predicates(const proto_msg::SparqlNode &node);
  void ensure_available_predicates_expr(const proto_msg::ExprNode &expr_node);
  void ensure_available_predicates_bgp(const proto_msg::BGPNode &bgp_node);
  void ensure_available_predicate(const proto_msg::RDFTerm &term);
  unsigned long get_timeout_ms();
  bool has_all_predicates_loaded(const std::vector<unsigned long> &predicates);
  I_CacheReplacement &get_replacement();
  I_CacheReplacement::REPLACEMENT_STRATEGY get_strategy_id();
  std::vector<unsigned long> extract_loaded_predicates_from_sequence(
      const std::vector<unsigned long> &input_predicates_ids);
};

#endif // RDFCACHEK2_CACHE_HPP
