//
// Created by cristobal on 3/2/22.
//

#ifndef RDFCACHEK2_STREAMERFROMCACHEDSOURCE_HPP
#define RDFCACHEK2_STREAMERFROMCACHEDSOURCE_HPP

#include "Cache.hpp"
#include "CachedSourceScanner.hpp"
#include "I_CachedPredicateSource.hpp"
#include "I_TRMatchingStreamer.hpp"
#include "TriplePatternQuery.hpp"

class StreamerFromCachedSource : public I_TRMatchingStreamer {
  I_CachedPredicateSource *cached_source;
  int channel_id;
  int pattern_channel_id;
  //  const proto_msg::TripleNodeIdEnc triple_pattern_node;
  TriplePatternQuery triple_pattern_query;
  unsigned long threshold_part_size;
  bool finished;
  bool first;

  bool subject_variable;
  bool object_variable;

  static constexpr long NODE_ANY = -9;

  std::unique_ptr<CachedSourceScanner> cached_source_scanner;

public:
  explicit StreamerFromCachedSource(I_CachedPredicateSource *cached_source,
                                    int channel_id,
                                    int current_pattern_channel_id,
                                    TriplePatternQuery triple_pattern_query,
                                    unsigned long threshold_part_size);
  proto_msg::CacheResponse get_next_response() override;
  int get_pattern_channel_id() override;
  int get_channel_id() override;
  bool all_sent() override;
  void set_finished();
};

#endif // RDFCACHEK2_STREAMERFROMCACHEDSOURCE_HPP
