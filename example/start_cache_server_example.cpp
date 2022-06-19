//
// Created by Cristobal Miranda, 2020
//

#include "server/CacheServer.hpp"
#include <Cache.hpp>

int main() {
  Cache cache;
  CacheServer cacheServer(cache, 8909, 1);

  cacheServer.start();
}