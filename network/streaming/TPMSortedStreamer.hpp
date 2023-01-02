//
// Created by cristobal on 04-11-22.
//

#ifndef RDFCACHEK2_TPMSORTEDSTREAMER_HPP
#define RDFCACHEK2_TPMSORTEDSTREAMER_HPP

#include "CacheContainer.hpp"
#include "I_TRMatchingStreamer.hpp"
#include "nodeids/TripleNodeId.hpp"
namespace k2cache {

class TPMSortedStreamer : public I_TRMatchingStreamer {
  int channel_id;
  int pattern_channel_id;
  const TripleNodeId &triple_pattern_node;
  CacheContainer *cache;
  uint64_t threshold_part_size;

  struct DataResults {
    std::vector<std::pair<uint64_t, uint64_t>> pair_results;
    std::vector<uint64_t> single_results;
  };

  DataResults data_results;
  uint64_t current_sent;

public:
  proto_msg::CacheResponse get_next_response() override;
  int get_pattern_channel_id() override;
  int get_channel_id() override;
  bool all_sent() override;

  TPMSortedStreamer(int channel_id, int pattern_channel_id,
                    const TripleNodeId &triple_pattern_node,
                    CacheContainer *cache, uint64_t threshold_part_size);
  void initialize();

private:
  void get_all_data(K2TreeScanner &scanner, bool subject_variable,
                    bool object_variable);
  void sort_data();
  proto_msg::CacheResponse get_next_pairs_response();
  proto_msg::CacheResponse get_next_single_response();
  proto_msg::CacheResponse get_empty_response();
};

} // namespace k2cache

#endif // RDFCACHEK2_TPMSORTEDSTREAMER_HPP
