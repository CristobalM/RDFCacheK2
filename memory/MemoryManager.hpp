//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_MEMORYMANAGER_HPP
#define RDFCACHEK2_MEMORYMANAGER_HPP

extern "C" {
#include <bitvector.h>

#include <block.h>
#include <memalloc.h>

#include <definitions.h>
}

#include <vector>

#include "MemoryPool.hpp"

#include <map>
#include <mutex>
#include <set>
#include <string>

class MemoryManager {

  uint64_t current_index;

  std::mutex m;

protected:
  MemoryManager();

  static MemoryManager _instance;

public:
  MemoryPool<struct block> blocks;
  MemoryPool<uint32_t[32]> containers_32;
  MemoryPool<uint32_t[64]> containers_64;
  MemoryPool<uint32_t[128]> containers_128;
  MemoryPool<uint32_t[256]> containers_256;
  MemoryPool<uint32_t[512]> containers_512;
  MemoryPool<uint32_t[1024]> containers_1024;

  std::map<uint32_t *, std::unique_ptr<uint32_t[]>> other_containers;

  uint64_t new_block_index();

  static MemoryManager &instance();

  std::string memory_usage();
};

#endif // RDFCACHEK2_MEMORYMANAGER_HPP
