//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHESERVER_HPP
#define RDFCACHEK2_CACHESERVER_HPP

#include <memory>
#include <thread>

#include <Cache.hpp>
#include "CacheServerTaskProcessor.hpp"
#include "TCPServerConnection.hpp"
#include "ServerWorker.hpp"


class CacheServer {
  Cache &cache;
  uint16_t port;
  uint8_t workers_count;
  CacheServerTaskProcessor task_processor;

  using connection_t = TCPServerConnection<CacheServerTaskProcessor>;
  using worker_t = ServerWorker<CacheServerTaskProcessor>;

  std::unique_ptr<connection_t> connection;
  std::vector<std::unique_ptr<worker_t>> workers;

public:

  explicit CacheServer(Cache &cache, uint16_t port, uint8_t workers_count);

  void start();
  void stop();

private:
  void main_thread_op();

};


#endif //RDFCACHEK2_CACHESERVER_HPP
