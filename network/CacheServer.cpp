//
// Created by Cristobal Miranda, 2020
//

#include "CacheServer.hpp"


void CacheServer::start() {
  connection = std::make_unique<connection_t>(port, task_processor);
  // Start workers pool
  for(auto i = 0; i < workers_count; i++){
    workers.emplace_back(std::make_unique<worker_t>(connection->get_processor()));
  }
  // Start server
  main_thread_op();
}

void CacheServer::stop() {
  connection->stop();
}

void CacheServer::main_thread_op() {
  connection->start();
}

CacheServer::CacheServer(Cache &cache, uint16_t port, uint8_t workers_count) :
cache(cache),
port(port),
workers_count(workers_count),
task_processor(cache)
{}
