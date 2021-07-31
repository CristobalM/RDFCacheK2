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

  auto time_control = std::make_unique<TimeControl>(
      1'000, std::chrono::milliseconds(cache.get_timeout_ms()));
  auto query_result_it = cache.run_query(tree, *time_control);
  begin_streaming_results(query_result_it, std::move(time_control));
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
  send_response(next_response);
  if (streamer.all_sent()) {
    task_processor.clean_streamer(id);
  }
}
void ServerTask::begin_streaming_results(
    std::shared_ptr<QueryResultIterator> query_result_iterator,
    std::unique_ptr<TimeControl> &&time_control) {
  auto &streamer = task_processor.create_streamer(
      std::move(query_result_iterator), std::move(time_control));
  auto first_response = streamer.get_next_response();
  send_response(first_response);
  if (streamer.all_sent()) {
    task_processor.clean_streamer(streamer.get_id());
  }
}
