//
// Created by Cristobal Miranda, 2020
//

#include "CacheServer.hpp"
namespace k2cache {
void CacheServer::start() {
  connection = std::make_unique<connection_t>(port, task_processor);
  // Start workers pool
  task_processor.start_workers(*connection);
  // Start server
  main_thread_op();
}

void CacheServer::stop() { connection->stop(); }

void CacheServer::main_thread_op() { connection->start(); }

CacheServer::CacheServer(CacheContainer &cache, uint16_t port,
                         uint8_t workers_count)
    : port(port), task_processor(cache, workers_count) {}
} // namespace k2cache
