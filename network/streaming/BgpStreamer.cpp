//
// Created by cristobal on 28-05-23.
//

#include "BGPStreamer.hpp"
#include "nodeids/node_ids_constants.hpp"

namespace k2cache {

proto_msg::CacheResponse BGPStreamer::get_next_message() {
  if (!query_iterator) {
    query_iterator = processor->execute_it();
    permutation_vec = processor->get_permutation_vec();
  }

  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(proto_msg::MessageType::RESPONSE_BGP_JOIN);
  auto *resp = cache_response.mutable_bgp_join_response();
  resp->set_channel_id(channel_id);

  auto *rows = resp->mutable_bgp_response_row();

  const int max_batch_count = 10'000'000;
  int batch_count = 0;
  const auto& nim = cache.get_nodes_ids_manager();
  while (query_iterator->has_next()) {
    auto row = query_iterator->next();
    auto *values = rows->Add()->mutable_bgp_response_row_values();

    for(size_t i = 0; i < row.size(); i++){
      auto cache_value = row[permutation_vec[i]];
      int err = 0;
      auto real_value = nim.get_real_id(cache_value, &err);
      if(err != (int)NidsErrCode::SUCCESS_ERR_CODE){
        throw std::runtime_error("ERROR: NOT FOUND REAL ID FOR CACHE VALUE" + std::to_string(cache_value));
      }
      values->Add(real_value);
    }
    batch_count++;
    if(!sent_first_batch && first_batch_small && batch_count >= 100){
      break;
    }
    if (batch_count >= max_batch_count) {
      break;
    }
  }
  sent_first_batch = true;
  resp->set_is_last(!query_iterator->has_next());

  return cache_response;
}

BGPStreamer::BGPStreamer(int channel_id, BGPMessage message,
                         CacheContainer &cache, bool first_batch_small)
    : channel_id(channel_id),
      cache(cache),
      first_batch_small(first_batch_small),
      var_index_manager(std::make_unique<VarIndexManager>()),
      time_control(std::make_unique<TimeControl>(
          1000, std::chrono::milliseconds(cache.get_timeout_ms()))),
      processor(std::make_unique<BGPProcessor>(
          std::move(message), cache.get_pcm(), cache.get_nodes_ids_manager(),
          *var_index_manager, *time_control)),
      query_iterator(nullptr) {}
int BGPStreamer::get_channel_id() { return channel_id; }
void BGPStreamer::cancel_query() {
  time_control->force_cancel();
}

} // namespace k2cache