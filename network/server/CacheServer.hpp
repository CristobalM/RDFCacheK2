//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHESERVER_HPP
#define RDFCACHEK2_CACHESERVER_HPP

#include <memory>
#include <thread>

#include "Cache.hpp"
#include "ServerWorker.hpp"
#include "server/conn/TCPServerConnection.hpp"
#include "server/tasks/CacheServerTaskProcessor.hpp"
namespace k2cache {
class CacheServer {
  uint16_t port;
  CacheServerTaskProcessor task_processor;

  using connection_t = TCPServerConnection<CacheServerTaskProcessor>;

  std::unique_ptr<connection_t> connection;

public:
  explicit CacheServer(Cache &cache, uint16_t port, uint8_t workers_count);

  void start();
  void stop();

private:
  void main_thread_op();
};
} // namespace k2cache
#endif // RDFCACHEK2_CACHESERVER_HPP
