//
// Created by cristobal on 9/1/21.
//

#ifndef RDFCACHEK2_TRIPLEPATTERNMATCHINGSTREAMER_HPP
#define RDFCACHEK2_TRIPLEPATTERNMATCHINGSTREAMER_HPP

#include "I_TRMatchingStreamer.hpp"
#include "CachingTripleTraversalCondition.hpp"
#include "CachedK2TreeScanner.hpp"
#include <Cache.hpp>
#include <sparql_tree.pb.h>
class TriplePatternMatchingStreamer : public I_TRMatchingStreamer {

  int channel_id;
  int pattern_channel_id;

  proto_msg::TripleNodeIdEnc triple_pattern_node;

  Cache *cache;
  unsigned long threshold_part_size;
  CachingTripleTraversalCondition caching_condition;

  bool first;
  std::unique_ptr<K2TreeScanner> k2tree_scanner;
  std::unique_ptr<CachedK2TreeScanner::Builder> cached_scanner_builder;

  bool subject_variable;
  bool object_variable;

  bool finished;


  bool should_cache;


  static constexpr long NODE_ANY = -9;

public:
  TriplePatternMatchingStreamer(
      int channel_id, int pattern_channel_id,
      const proto_msg::TripleNodeIdEnc triple_pattern_node, Cache *cache,
      unsigned long threshold_part_size,
      CachingTripleTraversalCondition caching_condition);
  TriplePatternMatchingStreamer(
      int channel_id, int pattern_channel_id,
      const proto_msg::TripleNodeIdEnc triple_pattern_node, Cache *cache,
      unsigned long threshold_part_size,
      CachingTripleTraversalCondition caching_condition,
      std::unique_ptr<K2TreeScanner> &&scanner
      );
  proto_msg::CacheResponse get_next_response() override;
  int get_pattern_channel_id() override;
  int get_channel_id() override;
  bool all_sent() override;
  void initialize_scanner();
  proto_msg::RDFTerm resource_to_term(RDFResource &&resource);
  void set_finished();
  proto_msg::CacheResponse timeout_proto_response();
  std::unique_ptr<K2TreeScanner> grab_cached_scanner();
};

#endif // RDFCACHEK2_TRIPLEPATTERNMATCHINGSTREAMER_HPP
