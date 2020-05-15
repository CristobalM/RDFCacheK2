//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_TCPSERVERCONNECTION_HPP
#define RDFCACHEK2_TCPSERVERCONNECTION_HPP


#include <cstdint>
#include <vector>
#include <bits/unique_ptr.h>



template <class TCPServerTaskProcessor>
class TCPServerConnection {
  int server_fd;
  uint16_t port;

  TCPServerTaskProcessor task_processor;

public:
  explicit TCPServerConnection(uint16_t port);

  void start();


};


#endif //RDFCACHEK2_TCPSERVERCONNECTION_HPP
