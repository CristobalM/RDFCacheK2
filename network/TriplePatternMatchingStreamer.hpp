//
// Created by cristobal on 9/1/21.
//

#ifndef RDFCACHEK2_TRIPLEPATTERNMATCHINGSTREAMER_HPP
#define RDFCACHEK2_TRIPLEPATTERNMATCHINGSTREAMER_HPP

#include "I_TRMatchingStreamer.hpp"
#include <Cache.hpp>
#include <sparql_tree.pb.h>
class TriplePatternMatchingStreamer : public I_TRMatchingStreamer {

  int channel_id;
  int pattern_channel_id;

  proto_msg::TripleNode triple_pattern_node;
  Cache *cache;
  TimeControl *time_control;
  unsigned long threshold_part_size;

  bool first;
  std::unique_ptr<K2TreeScanner> k2tree_scanner;

  bool subject_variable;
  bool object_variable;

  bool finished;

public:
  TriplePatternMatchingStreamer(int channel_id, int pattern_channel_id,
                                proto_msg::TripleNode triple_pattern_node,
                                Cache *cache, TimeControl *time_control,
                                unsigned long threshold_part_size);
  proto_msg::CacheResponse get_next_response() override;
  int get_pattern_channel_id() override;
  int get_channel_id() override;
  bool all_sent() override;
  void initialize_scanner();
  proto_msg::RDFTerm resource_to_term(RDFResource &&resource);
  void set_finished();
  proto_msg::CacheResponse timeout_proto_response();
  proto_msg::BasicType basic_type_from_data_type(ExprDataType type);
};

#endif // RDFCACHEK2_TRIPLEPATTERNMATCHINGSTREAMER_HPP
