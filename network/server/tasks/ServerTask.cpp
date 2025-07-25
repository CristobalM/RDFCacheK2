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

#include "BGPMessage.hpp"
#include "hashing.hpp"
#include "messages/utils.hpp"
#include "serialization_util.hpp"

using namespace std::chrono_literals;

namespace k2cache {

ServerTask::ServerTask(std::unique_ptr<ClientReqHandler> &&req_handler,
                       CacheContainer &cache, TaskProcessor &task_processor)
    : req_handler(std::move(req_handler)), cache(cache),
      task_processor(task_processor), finished(false) {}

void ServerTask::process_next() {
  auto message = req_handler->get_next_message();
  if (!message) {
    std::cout << "NULL message terminating server task..." << std::endl;
    finished = true;
    return;
  }
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
    std::cout << "Request of type SYNC_LOGS_WITH_INDEXES_REQUEST" << std::endl;
    process_sync_logs_with_indexes(*message);
    break;
  case proto_msg::MessageType::REQUEST_BGP_JOIN:
    std::cout << "Request of type REQUEST_BGP_JOIN" << std::endl;
    process_request_bgp_join(*message);
    break;
  case proto_msg::MessageType::REQUEST_MORE_BGP_JOIN:
    std::cout << "Request of type REQUEST_MORE_BGP_JOIN" << std::endl;
    process_request_more_bgp_join(*message);
    break;
  case proto_msg::MessageType::CANCEL_QUERY:
    std::cout << "Request of type CANCEL_QUERY" << std::endl;
    process_request_cancel_query(*message);
    break;
  default:
    std::cout << "received unknown message... ignoring " << std::endl;
    break;
  }
}
void ServerTask::process() {
  while (!finished) {
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
  //  std::cout << "sending result of " << result.size() << " bytes" <<
  //  std::endl;
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

static BGPNode proto_to_bgp_node(const proto_msg::NodePattern &pattern) {
  BGPNode node;
  if (pattern.has_variable_name()) {
    node.is_concrete = false;
    node.var_name = pattern.variable_name();
  } else {
    node.is_concrete = true;
    node.real_node_id = pattern.concrete_node_id();
  }
  return node;
}

#ifdef CACHE_DEBUG
static void print_bgp_join_message(const proto_msg::CacheResponse &response) {
  const auto &bgp_resp = response.bgp_join_response();
  std::cout << "is_last: " << (bgp_resp.is_last() ? "TRUE" : "FALSE")
            << std::endl;
  std::cout << "channel id: " << bgp_resp.channel_id() << std::endl;
  std::cout << "var names size: " << bgp_resp.var_names_size() << std::endl;

  for (int i = 0; i < bgp_resp.var_names_size(); i++) {
    std::cout << bgp_resp.var_names(i) << " ";
  }
  std::cout << std::endl;
  std::cout << "----------------" << std::endl;

  std::cout << "rows num: " << bgp_resp.bgp_response_row_size() << std::endl;
  for (int i = 0; i < bgp_resp.bgp_response_row_size(); i++) {
    const auto &row = bgp_resp.bgp_response_row(i);
    for (int j = 0; j < row.bgp_response_row_values_size(); j++) {
      std::cout << row.bgp_response_row_values(j) << " ";
    }
    std::cout << std::endl;
  }
}
#endif

void ServerTask::process_request_bgp_join(Message &message) {
  const auto &bgp_join = message.get_cache_request().bgp_join();

  BGPMessage bgp_message{};
  bgp_message.var_names.reserve(bgp_join.variable_names_size());
  for (int i = 0; i < bgp_join.variable_names_size(); i++) {
    bgp_message.var_names.push_back(bgp_join.variable_names(i));
  }
  bgp_message.patterns.reserve(bgp_join.triple_patterns_size());
  for (int i = 0; i < bgp_join.triple_patterns_size(); i++) {
    const auto &triple_pattern = bgp_join.triple_patterns(i);
    BGPTriple bgp_triple{};
    BGPNode subject = proto_to_bgp_node(triple_pattern.subject_node_pattern());
    BGPNode predicate =
        proto_to_bgp_node(triple_pattern.predicate_node_pattern());
    BGPNode object = proto_to_bgp_node(triple_pattern.object_node_pattern());
    bgp_triple.subject = std::move(subject);
    bgp_triple.predicate = std::move(predicate);
    bgp_triple.object = std::move(object);
    bgp_message.patterns.push_back(std::move(bgp_triple));
  }
  bgp_message.first_batch_small = bgp_join.first_small();
  auto &bgp_streamer = task_processor.get_bgp_streamer(std::move(bgp_message));
  std::cout << "started bgp streamer with channel id: " << bgp_streamer.get_channel_id() << std::endl;
  auto next_message = bgp_streamer.get_next_message();

#ifdef CACHE_DEBUG
  std::cout << "CACHE_DEBUG_START:: bgp_join_msg "
               "==========================================="
            << std::endl;
  print_bgp_join_message(next_message);
  std::cout << "CACHE_DEBUG_END:: bgp_join_msg "
               "==========================================="
            << std::endl;
#endif

  send_response(next_message);
  if (next_message.bgp_join_response().is_last()) {
    std::cout << "cleaning up bgp streamer first go, channel_id: " << bgp_streamer.get_channel_id() << std::endl;
    task_processor.clean_bgp_streamer(bgp_streamer.get_channel_id());
  }
}

void ServerTask::process_request_more_bgp_join(Message &message) {
  const auto &req = message.get_cache_request().continue_bgp_join();
  auto channel_id = req.channel_id();
  auto *streamer = task_processor.get_existing_bgp_streamer((int)channel_id);
  if(!streamer){
    proto_msg::CacheResponse cache_response;
    cache_response.set_response_type(proto_msg::MessageType::BAD_REQ);
    send_response(cache_response);
    return;
  }
  auto next_message = streamer->get_next_message();
#ifdef CACHE_DEBUG
  std::cout << "CACHE_DEBUG_START:: more_bgp_join_msg "
               "==========================================="
            << std::endl;
  print_bgp_join_message(next_message);
  std::cout << "CACHE_DEBUG_END:: more_bgp_join_msg "
               "==========================================="
            << std::endl;
#endif
  send_response(next_message);
  if (next_message.bgp_join_response().is_last()) {
    std::cout << "cleaning up bgp streamer with channel_id: " << streamer->get_channel_id() << std::endl;
    task_processor.clean_bgp_streamer(streamer->get_channel_id());
  }
}
void ServerTask::process_request_cancel_query(Message &message) {
  const auto &req = message.get_cache_request().cancel_query();
  auto channel_id = req.channel_id();
  auto *streamer = task_processor.get_existing_bgp_streamer((int)channel_id);
  if(streamer){
    std::cout << "process_request_cancel_query: cancelling query with id " << channel_id << std::endl;
    streamer->cancel_query();
  }
  else {
    std::cout << "process_request_cancel_query: not found streamer with id " << channel_id << std::endl;
  }
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(proto_msg::MessageType::ACK_CANCEL_QUERY);
  cache_response.mutable_ack_cancel_query()->set_channel_id(channel_id);
  send_response(cache_response);
}

} // namespace k2cache
