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
#include <query_processing/QueryResultIterator.hpp>
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

    std::cout << "Incoming request..." << std::endl;

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

    std::cout << "Incoming message of size: " << message.get_size()
              << std::endl;

    switch (message.request_type()) {
    case proto_msg::MessageType::UNKNOWN:
      break;
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

  {
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
  std::cout << "Sending message of size " << serialized.size() << " with hash '"
            << md5_human_readable(serialized_hash) << "'" << std::endl;
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
    std::shared_ptr<QueryResultIterator> query_result_iterator,
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
