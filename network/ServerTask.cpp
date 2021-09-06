//
// Created by Cristobal Miranda, 2020
//

#include <chrono>
#include <iostream>
#include <netinet/in.h>
#include <set>
#include <sstream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "ServerTask.hpp"
#include "TimeControl.hpp"
#include "network_msg_definitions.hpp"

#include <message_type.pb.h>
#include <response_msg.pb.h>

#include <condition_variable>
#include <hashing.hpp>
#include <query_processing/QueryResultIteratorHolder.hpp>
#include <serialization_util.hpp>

using namespace std::chrono_literals;

ServerTask::ServerTask(int client_socket_fd, Cache &cache,
                       TaskProcessor &task_processor)
    : client_socket_fd(client_socket_fd), cache(cache),
      task_processor(task_processor) {}

bool read_nbytes_from_socket(int client_socket_fd, char *read_buffer,
                             size_t bytes_to_read) {
  std::size_t offset = 0;
  for (;;) {
    ssize_t bytes_read = recv(client_socket_fd, read_buffer + offset,
                              bytes_to_read - offset, MSG_WAITALL);
    if (bytes_read < 0) {
      if (errno != EINTR) {
        std::cerr << "IO Error while reading from socket" << std::endl;
        return false;
      }
    } else if (bytes_read == 0) {
      if (offset == 0) {
        std::cerr << "Unexpected end of stream with offset 0" << std::endl;
        return false;
      } else {
        std::cerr << "Unexpected end of stream" << std::endl;
        return false;
      }
    } else if (offset + bytes_read == bytes_to_read) {
      return true;
    } else {
      offset += bytes_read;
    }
  }
}

void ServerTask::process() {

  for (;;) {
    uint32_t msg_size = 0;

    bool was_read = read_nbytes_from_socket(client_socket_fd,
                                            reinterpret_cast<char *>(&msg_size),
                                            sizeof(msg_size));
    if (!was_read) {
      std::cerr << "Error while reading msg_size data from connection"
                << std::endl;
      return;
    }

    msg_size = ntohl(msg_size);
    Message message(msg_size);

    was_read = read_nbytes_from_socket(client_socket_fd, message.get_buffer(),
                                       message.get_size());
    if (!was_read) {
      std::cerr << "Error while reading data from connection" << std::endl;
      return;
    }

    message.deserialize();

    switch (message.request_type()) {
    case proto_msg::MessageType::RUN_QUERY:
      std::cout << "Request of type RUN_QUERY" << std::endl;
      process_cache_query(message);
      break;
    case proto_msg::MessageType::CONNECTION_END:
      std::cout << "Request of type CONNECTION_END" << std::endl;
      process_connection_end();
      return;
    case proto_msg::MessageType::RECEIVE_REMAINING_RESULT:
      std::cout << "Request of type RECEIVE_REMAINING_RESULT" << std::endl;
      process_receive_remaining_result(message);
      break;
    case proto_msg::MessageType::CACHE_REQUEST_SEPARATE_PREDICATES:
      std::cout << "Request of type CACHE_REQUEST_SEPARATE_PREDICATES"
                << std::endl;
      process_predicates_lock_for_triple_stream(message);
      break;
    case proto_msg::MessageType::STREAM_REQUEST_TRIPLE_PATTERN:
      process_stream_request_triple_pattern(message);
      break;
    case proto_msg::MessageType::STREAM_CONTINUE_TRIPLE_PATTERN:
      process_stream_continue_triple_pattern(message);
      break;
    case proto_msg::MessageType::DONE_WITH_PREDICATES_NOTIFY:
      std::cout << "Request of type DONE_WITH_PREDICATES_NOTIFY" << std::endl;
      process_done_with_predicates_notify(message);
      break;
    case proto_msg::MessageType::CACHE_REQUEST_START_UPDATE_TRIPLES:
      std::cout << "Request of type CACHE_REQUEST_START_UPDATE_TRIPLES"
                << std::endl;
      process_request_start_update_triples(message);
      break;

    case proto_msg::MessageType::CACHE_DONE_UPDATE_TRIPLES:
      std::cout << "Request of type CACHE_DONE_UPDATE_TRIPLES" << std::endl;
      process_done_update_triples(message);
      break;

    case proto_msg::MessageType::TRIPLES_UPDATE_BATCH:
      std::cout << "Request of type TRIPLES_UPDATE_BATCH" << std::endl;
      process_update_triples_batch(message);
      break;

    default:
      std::cout << "received unknown message... ignoring " << std::endl;
      break;
    }
  }
}

int ServerTask::get_client_socket_fd() { return client_socket_fd; }

Cache &ServerTask::get_cache() { return cache; }

void ServerTask::process_cache_query(Message &message) {
  auto &tree =
      message.get_cache_request().cache_run_query_algebra().sparql_tree();
  auto valid = cache.query_is_valid(tree);

  if (!valid) {
    send_invalid_response();
    return;
  }

  auto predicates_in_query = get_predicates_in_query(tree.root());

  if (cache.get_strategy_id() !=
      I_CacheReplacement::REPLACEMENT_STRATEGY::NO_CACHING) {
    auto &replacement_mutex = cache.get_replacement().get_replacement_mutex();
    // auto &replacement_mutex = task_processor.get_replacement_mutex();
    std::lock_guard lg(replacement_mutex);
    if (!cache.has_all_predicates_loaded(*predicates_in_query)) {
      send_cache_miss_response();
      task_processor.process_missed_predicates(std::move(predicates_in_query));
      // starts a task which also locks replacement_mutex
      return;
    }

    if (!predicates_in_query->empty())
      task_processor.mark_using(*predicates_in_query);
  }

  auto time_control = std::make_unique<TimeControl>(
      1'000, std::chrono::milliseconds(cache.get_timeout_ms()));
  auto query_result_it = cache.run_query(tree, *time_control);
  begin_streaming_results(query_result_it, std::move(time_control),
                          std::move(predicates_in_query));
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
  send(client_socket_fd, result.data(), result.size() * sizeof(char), 0);
}
void ServerTask::process_connection_end() {
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(proto_msg::MessageType::CONNECTION_END);
  cache_response.mutable_connection_end_response()->set_end(true);
  send_response(cache_response);
}
void ServerTask::process_receive_remaining_result(Message &message) {
  int id = message.get_cache_request().receive_remaining_result().id();
  if (!task_processor.has_streamer(id)) {
    std::cout << "not found streamer with id " << id << std::endl;
    send_invalid_response();
    return;
  }
  auto &streamer = task_processor.get_streamer(id);
  auto next_response = streamer.get_next_response();
  if (streamer.all_sent()) {
    task_processor.clean_streamer(id);
  }
  send_response(next_response);
}
void ServerTask::begin_streaming_results(
    std::shared_ptr<QueryResultIteratorHolder> query_result_iterator,
    std::unique_ptr<TimeControl> &&time_control,
    std::shared_ptr<const std::vector<unsigned long>> predicates_in_use) {
  auto &streamer = task_processor.create_streamer(
      std::move(query_result_iterator), std::move(time_control),
      std::move(predicates_in_use));

  auto first_response = streamer.get_next_response();
  if (streamer.all_sent()) {
    task_processor.clean_streamer(streamer.get_id());
  }
  send_response(first_response);
}

void ServerTask::send_cache_miss_response() {
  std::cout << "cache miss... aborting" << std::endl;
  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(proto_msg::MessageType::CACHE_MISS);
  cache_response.error_response();
  send_response(cache_response);
}

std::shared_ptr<const std::vector<unsigned long>>
ServerTask::get_predicates_in_query(const proto_msg::SparqlNode &query_tree) {
  std::set<unsigned long> result;
  get_predicates_in_query_rec(query_tree, result);
  return std::make_shared<const std::vector<unsigned long>>(result.begin(),
                                                            result.end());
}
void ServerTask::get_predicates_in_query_rec(
    const proto_msg::SparqlNode &node, std::set<unsigned long> &result_set) {
  switch (node.node_case()) {

  case proto_msg::SparqlNode::kProjectNode:
    get_predicates_in_query_rec(node.project_node().sub_op(), result_set);
    break;
  case proto_msg::SparqlNode::kLeftJoinNode:
    get_predicates_in_query_rec(node.left_join_node().left_node(), result_set);
    get_predicates_in_query_rec(node.left_join_node().right_node(), result_set);
    break;
  case proto_msg::SparqlNode::kBgpNode:
    get_predicates_in_query_bgp(node.bgp_node(), result_set);
    break;
  case proto_msg::SparqlNode::kUnionNode: {
    for (int i = 0; i < node.union_node().nodes_list_size(); i++) {
      get_predicates_in_query_rec(node.union_node().nodes_list(i), result_set);
    }
  } break;
  case proto_msg::SparqlNode::kDistinctNode:
    get_predicates_in_query_rec(node.distinct_node().sub_node(), result_set);
    break;
  case proto_msg::SparqlNode::kOptionalNode:
    get_predicates_in_query_rec(node.optional_node().left_node(), result_set);
    get_predicates_in_query_rec(node.optional_node().right_node(), result_set);
    break;
  case proto_msg::SparqlNode::kMinusNode:
    get_predicates_in_query_rec(node.minus_node().left_node(), result_set);
    get_predicates_in_query_rec(node.minus_node().right_node(), result_set);
    break;
  case proto_msg::SparqlNode::kFilterNode:
    get_predicates_in_query_rec(node.filter_node().node(), result_set);
    break;
  case proto_msg::SparqlNode::kExtendNode:
    get_predicates_in_query_rec(node.extend_node().node(), result_set);
    break;
  case proto_msg::SparqlNode::kSequenceNode: {
    for (int i = 0; i < node.sequence_node().nodes_size(); i++) {
      get_predicates_in_query_rec(node.sequence_node().nodes(i), result_set);
    }
  } break;
  case proto_msg::SparqlNode::kSliceNode:
    get_predicates_in_query_rec(node.slice_node().node(), result_set);
    break;
  case proto_msg::SparqlNode::kOrderNode:
    get_predicates_in_query_rec(node.order_node().node(), result_set);
    break;
  default:
    break;
  }
}
void ServerTask::get_predicates_in_query_bgp(
    const proto_msg::BGPNode &bgp_node, std::set<unsigned long> &result_set) {
  for (int i = 0; i < bgp_node.triple_size(); i++) {
    const auto &triple = bgp_node.triple(i);
    auto predicate_id =
        cache.get_pcm().get_resource_index(RDFResource(triple.predicate()));
    if (predicate_id != 0)
      result_set.insert(predicate_id);
  }
}
void ServerTask::process_predicates_lock_for_triple_stream(Message &message) {
  const auto &sep_pred =
      message.get_cache_request().cache_request_separate_predicates();

  std::vector<unsigned long> predicates_requested;
  for (int i = 0; i < sep_pred.predicates_size(); i++) {
    const auto &pred_term = sep_pred.predicates(i);
    auto resource_id =
        cache.get_pcm().get_resource_index(RDFResource(pred_term));
    if (resource_id > 0)
      predicates_requested.push_back(resource_id);
  }

  std::vector<unsigned long> loaded_predicates;

  if (cache.get_strategy_id() !=
      I_CacheReplacement::REPLACEMENT_STRATEGY::NO_CACHING) {
    auto &replacement_mutex = cache.get_replacement().get_replacement_mutex();
    std::lock_guard lg(replacement_mutex);

    loaded_predicates =
        cache.extract_loaded_predicates_from_sequence(predicates_requested);

    if (!loaded_predicates.empty())
      task_processor.mark_using(loaded_predicates);

    if (loaded_predicates.size() < predicates_requested.size()) {

      std::set<unsigned long> requested_set(predicates_requested.begin(),
                                            predicates_requested.end());
      std::set<unsigned long> loaded_set(loaded_predicates.begin(),
                                         loaded_predicates.end());
      std::set<unsigned long> difference;

      std::set_difference(requested_set.begin(), requested_set.end(),
                          loaded_set.begin(), loaded_set.end(),
                          std::inserter(difference, difference.begin()));

      task_processor.process_missed_predicates(
          std::make_shared<const std::vector<unsigned long>>(difference.begin(),
                                                             difference.end()));
    }
  } else {
    loaded_predicates = std::move(predicates_requested);
  }

  auto time_control = std::make_unique<TimeControl>(
      100'000, std::chrono::milliseconds(cache.get_timeout_ms()));

  auto &triples_streamer = task_processor.create_triples_streamer(
      std::move(loaded_predicates), std::move(time_control));

  auto response = triples_streamer.get_loaded_predicates_response();
  send_response(response);
}
void ServerTask::process_stream_request_triple_pattern(Message &message) {
  auto &s_req = message.get_cache_request().stream_request_triple_pattern();
  auto channel_id = s_req.channel_id();
  auto &triples_streamer = task_processor.get_triple_streamer(channel_id);
  auto &triple_pattern_streamer =
      triples_streamer.start_streaming_matching_triples(s_req.triple_node());
  auto cache_response = triple_pattern_streamer.get_next_response();
  // std::cout << "sending response for stream req: " <<
  // cache_response.DebugString() << std::endl;
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
  // auto &req =
  // message.get_cache_request().cache_request_start_update_triples();
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
  cache_response.set_response_type(proto_msg::MessageType::ACK_DONE_UPDATE_TRIPLES);
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
    updater.add_triple(RDFTripleResource(triple_proto));
  }
  for (const auto &triple_proto : triples_to_delete) {
    updater.delete_triple(RDFTripleResource(triple_proto));
  }

  proto_msg::CacheResponse cache_response;
  cache_response.set_response_type(proto_msg::BATCH_RECEIVED_RESPONSE);
  cache_response.mutable_batch_received_response()->set_update_id(update_id);
  send_response(cache_response);
}
