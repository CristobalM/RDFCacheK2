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
  unsigned long threshold_part_size;

  struct DataResults {
    std::vector<std::pair<unsigned long, unsigned long>> pair_results;
    std::vector<unsigned long> single_results;
  };

public:
  proto_msg::CacheResponse get_next_response() override;
  int get_pattern_channel_id() override;
  int get_channel_id() override;
  bool all_sent() override;

  TPMSortedStreamer(int channel_id, int pattern_channel_id,
                    const TripleNodeId &triple_pattern_node,
                    CacheContainer *cache, unsigned long threshold_part_size);
  void initialize();

private:
  DataResults get_all_data(K2TreeScanner &scanner, bool subject_variable,
                    bool object_variable);
};

} // namespace k2cache

#endif // RDFCACHEK2_TPMSORTEDSTREAMER_HPP
