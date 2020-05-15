//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_SERVERTASK_HPP
#define RDFCACHEK2_SERVERTASK_HPP


#include <memory>

class ServerTask {
  int client_socket_fd;
  std::unique_ptr<char[]> message;

public:
  explicit ServerTask(int client_socket_fd, std::unique_ptr<char[]> &&message);

};


#endif //RDFCACHEK2_SERVERTASK_HPP
