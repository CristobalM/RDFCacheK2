//
// Created by Cristobal Miranda, 2020
//

#include <condition_variable>
#include <iostream>
#include <set>
#include <sstream>
#include <thread>

#include "ServerTask.hpp"

#include "message_type.pb.h"
#include "response_msg.pb.h"

#include "hashing.hpp"
#include "messages/utils.hpp"
#include "serialization_util.hpp"

using namespace std::chrono_literals;

namespace k2cache {

ServerTask::ServerTask(std::unique_ptr<ClientReqHandler> &&req_handler,
                       CacheContainer &cache, TaskProcessor &task_processor)
    : req_handler(std::move(req_handler)), cache(cache),
      task_processor(task_processor) {}

void ServerTask::process_next() {
  auto message = req_handler->get_next_message();

  switch (message->request_type()) {
  case proto_msg::MessageType::CONNECTION_END:
    std::cout << "Request of type CONNECTION_END" << std::endl;
    process_connection_end();
    return;
  case proto_msg::MessageType::CACHE_REQUEST_SEPARATE_PREDICATES:
    std::cout << "Request of type CACHE_REQUEST_SEPARATE_PREDICATES"
              << std::endl;
    process_predicates_lock_for_triple_stream(*message);
    break;
  case proto_msg::MessageType::STREAM_REQUEST_TRIPLE_PATTERN:
    process_stream_request_triple_pattern(*message);
    break;
  case proto_msg::MessageType::STREAM_CONTINUE_TRIPLE_PATTERN:
    process_stream_continue_triple_pattern(*message);
    break;
  case proto_msg::MessageType::DONE_WITH_PREDICATES_NOTIFY:
    std::cout << "Request of type DONE_WITH_PREDICATES_NOTIFY" << std::endl;
    process_done_with_predicates_notify(*message);
    break;
  case proto_msg::MessageType::CACHE_REQUEST_START_UPDATE_TRIPLES:
    std::cout << "Request of type CACHE_REQUEST_START_UPDATE_TRIPLES"
              << std::endl;
    process_request_start_update_triples(*message);
    break;

  case proto_msg::MessageType::CACHE_DONE_UPDATE_TRIPLES:
    std::cout << "Request of type CACHE_DONE_UPDATE_TRIPLES" << std::endl;
    process_done_update_triples(*message);
    break;

  case proto_msg::MessageType::TRIPLES_UPDATE_BATCH:
    std::cout << "Request of type TRIPLES_UPDATE_BATCH" << std::endl;
    process_update_triples_batch(*message);
    break;

  case proto_msg::MessageType::SYNC_LOGS_WITH_INDEXES_REQUEST:
    std::cout << "Request of type SYNC_LOGS_WITH_INDEXES_REQUEST"
              << std::endl;
    process_sync_logs_with_indexes(*message);
    break;

  default:
    std::cout << "received unknown message... ignoring " << std::endl;
    break;
  }
}
void ServerTask::process() {
  for (;;) {
    process_next();
  }
}

void ServerTask::send_invalid_response() {
  std::cout << "invalid query... aborting" << std::endl;
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(
      proto_msg::MessageType::INVALID_QUERY_RESPONSE);
  cache_response.mutable_invalid_query_response();
  send_response(cache_response);
}

void ServerTask::send_response(proto_msg::CacheResponse &cache_response) {

  std::string serialized = cache_response.SerializeAsString();
  auto serialized_hash = md5calc(serialized);
  std::stringstream ss;
  //  std::cout << "Sending message of size " << serialized.size() << " with
  //  hash '"
  //            << md5_human_readable(serialized_hash) << "'" << std::endl;
  write_u64(ss, serialized.size());
  ss.write(serialized_hash.data(), sizeof(char) * serialized_hash.size());
  ss.write(serialized.data(), sizeof(char) * serialized.size());

  auto result = ss.str();
//  std::cout << "sending result of " << result.size() << " bytes" << std::endl;
  req_handler->send_response(result);
}
void ServerTask::process_connection_end() {
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(proto_msg::MessageType::CONNECTION_END);
  cache_response.mutable_connection_end_response()->set_end(true);
  send_response(cache_response);
}

void ServerTask::process_predicates_lock_for_triple_stream(Message &message) {
  const auto &sep_pred =
      message.get_cache_request().cache_request_separate_predicates();

  std::vector<uint64_t> predicates_requested;
  std::cout << "requested predicates: ";
  for (int i = 0; i < sep_pred.predicates_size(); i++) {
    const auto &pred_term = sep_pred.predicates(i);
    auto predicate_id_original = pred_term.encoded_data();
    auto translated_predicate_id =
        cache.get_nodes_ids_manager().get_id((long)predicate_id_original);

    predicates_requested.push_back(translated_predicate_id);
    std::cout << "(" << pred_term.encoded_data() << ", "
              << translated_predicate_id << "); ";
  }
  std::cout << std::endl;

  std::vector<uint64_t> loaded_predicates;

  if (cache.get_strategy_id() !=
      I_CacheReplacement::REPLACEMENT_STRATEGY::NO_CACHING) {
    auto &replacement_mutex = cache.get_replacement().get_replacement_mutex();
    std::lock_guard lg(replacement_mutex);

    loaded_predicates =
        cache.extract_loaded_predicates_from_sequence(predicates_requested);

    if (!loaded_predicates.empty())
      task_processor.mark_using(loaded_predicates);

    if (loaded_predicates.size() < predicates_requested.size()) {

      std::set<uint64_t> requested_set(predicates_requested.begin(),
                                            predicates_requested.end());
      std::set<uint64_t> loaded_set(loaded_predicates.begin(),
                                         loaded_predicates.end());
      std::set<uint64_t> difference;

      std::set_difference(requested_set.begin(), requested_set.end(),
                          loaded_set.begin(), loaded_set.end(),
                          std::inserter(difference, difference.begin()));

      task_processor.process_missed_predicates(
          std::make_shared<const std::vector<uint64_t>>(difference.begin(),
                                                             difference.end()));
    }
  } else {
    loaded_predicates = std::move(predicates_requested);
  }

  auto &triples_streamer =
      task_processor.create_triples_streamer(std::move(loaded_predicates));

  auto response = triples_streamer.get_loaded_predicates_response();
  send_response(response);
}
void ServerTask::process_stream_request_triple_pattern(Message &message) {
  auto &s_req = message.get_cache_request().stream_request_triple_pattern();
  auto channel_id = s_req.channel_id();
  auto &triples_streamer = task_processor.get_triple_streamer(channel_id);
  auto &triple_pattern_streamer =
      triples_streamer.start_streaming_matching_triples(
          proto_triple_to_internal(s_req.triple_node()));
  auto cache_response = triple_pattern_streamer.get_next_response();
  send_response(cache_response);
  if (triple_pattern_streamer.all_sent()) {
    triples_streamer.clean_pattern_streamer(
        triple_pattern_streamer.get_pattern_channel_id());
  }
}
void ServerTask::process_stream_continue_triple_pattern(Message &message) {
  auto &cs_req = message.get_cache_request().stream_continue_triple_pattern();
  auto channel_id = cs_req.channel_id();
  auto pattern_channel_id = cs_req.pattern_channel_id();
  auto &triples_streamer = task_processor.get_triple_streamer(channel_id);
  auto &triple_pattern_streamer =
      triples_streamer.get_triple_pattern_streamer(pattern_channel_id);
  auto cache_response = triple_pattern_streamer.get_next_response();
  send_response(cache_response);
  if (triple_pattern_streamer.all_sent()) {
    triples_streamer.clean_pattern_streamer(pattern_channel_id);
  }
}
void ServerTask::process_done_with_predicates_notify(Message &message) {
  auto &done_req = message.get_cache_request().done_with_predicates_notify();
  auto channel_id = done_req.channel_id();
  task_processor.clean_triple_streamer(channel_id);
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(
      proto_msg::MessageType::ACK_WITH_DONE_TRIPLES_STREAM);
  cache_response.mutable_ack_done_with_channel();
  send_response(cache_response);
}

void ServerTask::process_request_start_update_triples(Message &) {
  auto update_id = task_processor.begin_update_session();
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(
      proto_msg::MessageType::ACCEPT_UPDATE_REQUEST);
  cache_response.mutable_accept_update_request()->set_update_id(update_id);
  send_response(cache_response);
}
void ServerTask::process_done_update_triples(Message &message) {
  auto &req = message.get_cache_request().cache_done_update_triples();
  int update_id = req.update_id();
  auto &updater = task_processor.get_updater(update_id);
  updater.commit_updates();
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(
      proto_msg::MessageType::ACK_DONE_UPDATE_TRIPLES);
  cache_response.mutable_ack_done_update();
  send_response(cache_response);
}

void ServerTask::process_update_triples_batch(Message &message) {
  auto &req = message.get_cache_request().triples_update_batch();
  int update_id = req.update_id();
  auto &updater = task_processor.get_updater(update_id);
  const auto &triples_to_add = req.triples_to_add();
  const auto &triples_to_delete = req.triples_to_delete();

  for (const auto &triple_proto : triples_to_add) {
    updater.add_triple(proto_triple_to_internal(triple_proto));
  }
  for (const auto &triple_proto : triples_to_delete) {
    updater.delete_triple(proto_triple_to_internal(triple_proto));
  }

  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(proto_msg::BATCH_RECEIVED_RESPONSE);
  cache_response.mutable_batch_received_response()->set_update_id(update_id);
  send_response(cache_response);
}

void ServerTask::process_sync_logs_with_indexes(const Message &) {
  task_processor.sync_to_persistent();
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(proto_msg::SYNC_LOGS_WITH_INDEXES_RESPONSE);
  send_response(cache_response);
}
} // namespace k2cache
