//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_SERVERTASK_HPP
#define RDFCACHEK2_SERVERTASK_HPP

#include "Message.hpp"
#include "TaskProcessor.hpp"
#include <Cache.hpp>
#include <memory>
#include <response_msg.pb.h>

class ServerTask {
  int client_socket_fd;
  Cache &cache;
  TaskProcessor &task_processor;

public:
  ServerTask(int client_socket_fd, Cache &cache, TaskProcessor &task_processor);

  void process();

  int get_client_socket_fd();
  Cache &get_cache();

  void process_cache_query(Message &message);
  void send_invalid_response();

  void send_response(proto_msg::CacheResponse &cache_response);
  void process_connection_end();

  void process_receive_remaining_result(Message &message);
  void begin_streaming_results(
      std::shared_ptr<QueryResultIterator> query_result_iterator,
      std::unique_ptr<TimeControl> &&time_control);
};

#endif // RDFCACHEK2_SERVERTASK_HPP
