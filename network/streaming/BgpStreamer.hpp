//
// Created by cristobal on 28-05-23.
//

#ifndef RDFCACHEK2_BGPSTREAMER_HPP
#define RDFCACHEK2_BGPSTREAMER_HPP

#include "BgpMessage.hpp"
#include "CacheContainer.hpp"
#include "I_BgpStreamer.hpp"
#include "query_processing/BGPProcessor.hpp"
namespace k2cache {
class BgpStreamer : public I_BgpStreamer {
  int channel_id;
  CacheContainer &cache;

  std::unique_ptr<VarIndexManager> var_index_manager;
  std::unique_ptr<TimeControl> time_control;
  std::unique_ptr<BGPProcessor> processor;
  std::shared_ptr<QueryIterator> query_iterator;
public:
  BgpStreamer(int channel_id, BgpMessage message, CacheContainer &cache);
  proto_msg::CacheResponse get_next_message() override;
  int get_channel_id() override;
};
}

#endif // RDFCACHEK2_BGPSTREAMER_HPP
