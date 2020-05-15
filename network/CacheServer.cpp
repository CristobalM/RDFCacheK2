//
// Created by Cristobal Miranda, 2020
//

#include "CacheServer.hpp"


void CacheServer::start() {
  connection = std::make_unique<connection_t>(port);
}

void CacheServer::stop() {

}

void CacheServer::main_thread_op() {

}

CacheServer::CacheServer(Cache *cache, uint16_t port) : cache(cache), port(port) {
}
