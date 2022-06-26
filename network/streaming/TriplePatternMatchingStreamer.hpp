//
// Created by cristobal on 9/1/21.
//

#ifndef RDFCACHEK2_TRIPLEPATTERNMATCHINGSTREAMER_HPP
#define RDFCACHEK2_TRIPLEPATTERNMATCHINGSTREAMER_HPP

#include "I_TRMatchingStreamer.hpp"
#include "k2tree/RDFTriple.hpp"
#include <Cache.hpp>
#include <sparql_tree.pb.h>
namespace k2cache {
class TriplePatternMatchingStreamer : public I_TRMatchingStreamer {

  int channel_id;
  int pattern_channel_id;

  proto_msg::TripleNodeIdEnc triple_pattern_node;

  Cache *cache;
  unsigned long threshold_part_size;

  bool first;
  std::unique_ptr<K2TreeScanner> k2tree_scanner;

  bool subject_variable;
  bool object_variable;

  bool finished;

  static constexpr long NODE_ANY = -9;

public:
  TriplePatternMatchingStreamer(int channel_id, int pattern_channel_id,
                                proto_msg::TripleNodeIdEnc triple_pattern_node,
                                Cache *cache,
                                unsigned long threshold_part_size);
  proto_msg::CacheResponse get_next_response() override;
  int get_pattern_channel_id() override;
  int get_channel_id() override;
  bool all_sent() override;
  void initialize_scanner();
  proto_msg::RDFTerm resource_to_term(RDFResource &&resource);
  void set_finished();
  proto_msg::CacheResponse timeout_proto_response();
};
} // namespace k2cache
#endif // RDFCACHEK2_TRIPLEPATTERNMATCHINGSTREAMER_HPP
