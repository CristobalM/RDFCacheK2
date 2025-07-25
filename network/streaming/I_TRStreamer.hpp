//
// Created by cristobal on 8/31/21.
//

#ifndef RDFCACHEK2_I_TRSTREAMER_HPP
#define RDFCACHEK2_I_TRSTREAMER_HPP

#include "I_TRMatchingStreamer.hpp"
#include "nodeids/TripleNodeId.hpp"

namespace k2cache {
class I_TRStreamer {
public:
  virtual proto_msg::CacheResponse get_loaded_predicates_response() = 0;
  virtual I_TRMatchingStreamer &
  start_streaming_matching_triples(const TripleNodeId &triple_pattern) = 0;
  virtual int get_id() = 0;
  virtual const std::vector<uint64_t> &get_predicates_in_use() = 0;
  virtual bool is_done() = 0;

  virtual ~I_TRStreamer() = default;
  virtual void clean_pattern_streamer(int pattern_channel_id) = 0;
  virtual I_TRMatchingStreamer &
  get_triple_pattern_streamer(int pattern_channel_id) = 0;
};
} // namespace k2cache

#endif // RDFCACHEK2_I_TRSTREAMER_HPP
