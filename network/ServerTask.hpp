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
  proto_msg::CacheResponse
  create_response_from_query_result(std::shared_ptr<QueryResult> query_result,
                                    Message &message);

  void send_response(proto_msg::CacheResponse &cache_response);
  void process_connection_end();

  proto_msg::CacheResponse
  create_parts_response(std::set<uint64_t> &&keys,
                        std::shared_ptr<QueryResult> result);
  void process_receive_remaining_result(Message &message);
  void send_timeout();
};

#endif // RDFCACHEK2_SERVERTASK_HPP
