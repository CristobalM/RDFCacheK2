//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_SERVERTASK_HPP
#define RDFCACHEK2_SERVERTASK_HPP

#include "Message.hpp"
#include <Cache.hpp>
#include <memory>

class ServerTask {
  int client_socket_fd;
  Cache &cache;

public:
  explicit ServerTask(int client_socket_fd, Cache &cache);

  void process();

  int get_client_socket_fd();
  Cache &get_cache();
};

#endif // RDFCACHEK2_SERVERTASK_HPP
