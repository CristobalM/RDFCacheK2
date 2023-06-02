//
// Created by cristobal on 28-05-23.
//

#include "BgpStreamer.hpp"

namespace k2cache {

proto_msg::CacheResponse BgpStreamer::get_next_message() {
  if (!query_iterator) {
    query_iterator = processor->execute_it();
    permutation_vec = processor->get_permutation_vec();
  }

  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(proto_msg::MessageType::RESPONSE_BGP_JOIN);
  auto *resp = cache_response.mutable_bgp_join_response();
  resp->set_channel_id(channel_id);
  auto *rows = resp->mutable_bgp_response_row();

  const int max_batch_count = 10000;
  int batch_count = 0;
  while (query_iterator->has_next()) {
    auto row = query_iterator->next();
    auto *values = rows->Add()->mutable_bgp_response_row_values();

    for(size_t i = 0; i < row.size(); i++){
      values->Add(row[permutation_vec[i]]);
    }
    batch_count++;
    if (batch_count >= max_batch_count) {
      break;
    }
  }

  resp->set_is_last(!query_iterator->has_next());

  return cache_response;
}

BgpStreamer::BgpStreamer(int channel_id, BgpMessage message,
                         CacheContainer &cache)
    : channel_id(channel_id),
      var_index_manager(std::make_unique<VarIndexManager>()),
      time_control(std::make_unique<TimeControl>(
          1000, std::chrono::milliseconds(cache.get_timeout_ms()))),
      processor(std::make_unique<BGPProcessor>(
          std::move(message), cache.get_pcm(), cache.get_nodes_ids_manager(),
          *var_index_manager, *time_control)),
      query_iterator(nullptr) {}
int BgpStreamer::get_channel_id() { return channel_id; }

} // namespace k2cache