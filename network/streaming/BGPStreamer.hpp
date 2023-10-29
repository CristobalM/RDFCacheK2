//
// Created by cristobal on 28-05-23.
//

#ifndef RDFCACHEK2_BGPSTREAMER_HPP
#define RDFCACHEK2_BGPSTREAMER_HPP

#include "BGPMessage.hpp"
#include "CacheContainer.hpp"
#include "I_BGPStreamer.hpp"
#include "query_processing/BGPProcessor.hpp"
namespace k2cache {
class BGPStreamer : public I_BGPStreamer {
  int channel_id;
  CacheContainer &cache;
  bool first_batch_small;

  std::unique_ptr<VarIndexManager> var_index_manager;
  std::unique_ptr<TimeControl> time_control;
  std::unique_ptr<BGPProcessor> processor;
  std::shared_ptr<QueryIterator> query_iterator;


  std::vector<unsigned long> permutation_vec{};

  bool sent_first_batch;

public:
  BGPStreamer(int channel_id, BGPMessage message, CacheContainer &cache, bool first_batch_small);
  proto_msg::CacheResponse get_next_message() override;
  int get_channel_id() override;
  void cancel_query() override;
};
}

#endif // RDFCACHEK2_BGPSTREAMER_HPP
