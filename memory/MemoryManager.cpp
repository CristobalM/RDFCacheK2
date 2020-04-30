//
// Created by Cristobal Miranda, 2020
//

#include "MemoryManager.hpp"

const size_t default_pool_block_sz = 32;

MemoryManager::MemoryManager()
: blocks(default_pool_block_sz),
  block_topologies(default_pool_block_sz),
  block_frontiers(default_pool_block_sz),
  vectors(default_pool_block_sz),
  containers_32(default_pool_block_sz),
  containers_64(default_pool_block_sz),
  containers_128(default_pool_block_sz),
  containers_256(default_pool_block_sz),
  containers_512(default_pool_block_sz),
  containers_1024(default_pool_block_sz)
  {

}

MemoryManager &MemoryManager::instance() {
  return _instance;
}
