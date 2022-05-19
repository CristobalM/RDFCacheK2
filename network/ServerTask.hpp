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
#include <set>

class ServerTask {
  int client_socket_fd;
  Cache &cache;
  TaskProcessor &task_processor;

public:
  ServerTask(int client_socket_fd, Cache &cache, TaskProcessor &task_processor);

  void process();

  int get_client_socket_fd();
  Cache &get_cache();

  void send_invalid_response();

  void send_response(proto_msg::CacheResponse &cache_response);
  void process_connection_end();

  void process_predicates_lock_for_triple_stream(Message &message);
  void process_stream_request_triple_pattern(Message &message);
  void process_stream_continue_triple_pattern(Message &message);
  void process_done_with_predicates_notify(Message &message);
  void process_request_start_update_triples(Message &message);
  void process_done_update_triples(Message &message);
  void process_update_triples_batch(Message &message);
  void process_sync_logs_with_indexes(const Message &);
};

#endif // RDFCACHEK2_SERVERTASK_HPP
