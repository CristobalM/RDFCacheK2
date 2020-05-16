//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_SERVERTASK_HPP
#define RDFCACHEK2_SERVERTASK_HPP


#include <memory>
#include <Cache.hpp>
#include "Message.hpp"

class ServerTask {
  int client_socket_fd;
  Message message;
  Cache &cache;

public:
  explicit ServerTask(int client_socket_fd, Message &&message, Cache &cache);

  void process();

  int get_client_socket_fd();
  Message &get_message();
  Cache &get_cache();



};


#endif //RDFCACHEK2_SERVERTASK_HPP
