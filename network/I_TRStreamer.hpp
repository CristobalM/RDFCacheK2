//
// Created by cristobal on 8/31/21.
//

#ifndef RDFCACHEK2_I_TRSTREAMER_HPP
#define RDFCACHEK2_I_TRSTREAMER_HPP

#include "I_TRMatchingStreamer.hpp"
#include <response_msg.pb.h>
class I_TRStreamer {
public:
  virtual proto_msg::CacheResponse get_loaded_predicates_response() = 0;
  virtual I_TRMatchingStreamer &start_streaming_matching_triples(
      const proto_msg::TripleNode &triple_pattern) = 0;
  virtual int get_id() = 0;
  virtual const std::vector<unsigned long> &get_predicates_in_use() = 0;

  virtual ~I_TRStreamer() = default;
};

#endif // RDFCACHEK2_I_TRSTREAMER_HPP
