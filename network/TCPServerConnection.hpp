//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_TCPSERVERCONNECTION_HPP
#define RDFCACHEK2_TCPSERVERCONNECTION_HPP


#include <cstdint>
#include <vector>
#include <mutex>



template <class TCPServerTaskProcessor>
class TCPServerConnection {
  int server_fd;
  uint16_t port;

  TCPServerTaskProcessor &task_processor;

  bool running;
  std::mutex running_mutex;

public:
  TCPServerConnection(uint16_t port, TCPServerTaskProcessor &task_processor);

  void start();
  void stop();

  TCPServerTaskProcessor &get_processor();



private:

  bool is_running();
  void set_running(bool value);

};


#endif //RDFCACHEK2_TCPSERVERCONNECTION_HPP
