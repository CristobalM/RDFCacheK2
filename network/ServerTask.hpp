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

  void process_cache_query(Message &message);
  void send_invalid_response();

  void send_response(proto_msg::CacheResponse &cache_response);
  void process_connection_end();

  void process_receive_remaining_result(Message &message);
  void begin_streaming_results(
      std::shared_ptr<QueryResultIterator> query_result_iterator,
      std::unique_ptr<TimeControl> &&time_control,
      std::vector<unsigned long> &&predicates_in_use);
  std::vector<unsigned long>
  get_predicates_in_query(const proto_msg::SparqlNode &query_tree);
  void send_cache_miss_response();
  void get_predicates_in_query_rec(const proto_msg::SparqlNode &node,
                                   std::set<unsigned long> &result_set);
  void get_predicates_in_query_bgp(const proto_msg::BGPNode &bgp_node,
                                   std::set<unsigned long> &result_set);
};

#endif // RDFCACHEK2_SERVERTASK_HPP
