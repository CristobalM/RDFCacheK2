//
// Created by cristobal on 3/2/22.
//

#ifndef RDFCACHEK2_STREAMERFROMCACHEDSOURCE_HPP
#define RDFCACHEK2_STREAMERFROMCACHEDSOURCE_HPP

#include "Cache.hpp"
#include "I_TRMatchingStreamer.hpp"
#include "fic/I_CachedPredicateSource.hpp"
#include "scanner/CachedSourceScanner.hpp"
namespace k2cache {
class StreamerFromCachedSource : public I_TRMatchingStreamer {
  I_CachedPredicateSource *cached_source;
  int channel_id;
  int pattern_channel_id;
  const proto_msg::TripleNodeIdEnc triple_pattern_node;
  Cache *cache;
  unsigned long threshold_part_size;
  bool finished;
  bool first;

  bool subject_variable;
  bool object_variable;

  unsigned long translated_subject;
  unsigned long translated_object;

  static constexpr long NODE_ANY = -9;

  std::unique_ptr<CachedSourceScanner> cached_source_scanner;

public:
  explicit StreamerFromCachedSource(
      I_CachedPredicateSource *cached_source, int channel_id,
      int current_pattern_channel_id,
      proto_msg::TripleNodeIdEnc triple_pattern_node, Cache *cache,
      unsigned long threshold_part_size);
  proto_msg::CacheResponse get_next_response() override;
  int get_pattern_channel_id() override;
  int get_channel_id() override;
  bool all_sent() override;
  void set_finished();
};
} // namespace k2cache

#endif // RDFCACHEK2_STREAMERFROMCACHEDSOURCE_HPP
