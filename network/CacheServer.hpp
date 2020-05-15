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


class CacheServer {
  Cache *cache;
  uint16_t port;

  using connection_t = TCPServerConnection<CacheServerTaskProcessor>;

  std::unique_ptr<connection_t> connection;

public:

  explicit CacheServer(Cache *cache, uint16_t port);

  void start();
  void stop();

private:
  void main_thread_op();

};


#endif //RDFCACHEK2_CACHESERVER_HPP
