//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_MEMORYMANAGER_HPP
#define RDFCACHEK2_MEMORYMANAGER_HPP

extern "C" {
#include <bitvector.h>
#include <vector.h>

#include <block_frontier.h>
#include <block_topology.h>
#include <block.h>

#include <memalloc.h>

#include <definitions.h>
}

#include <vector>

#include "MemoryPool.hpp"

class MemoryManager {



protected:
  MemoryManager();

  static MemoryManager _instance;

public:
  MemoryPool<struct block> blocks;
  MemoryPool<struct block_topology> block_topologies;
  MemoryPool<struct block_frontier> block_frontiers;
  MemoryPool<struct vector> vectors;
  MemoryPool<struct bitvector> bitvectors;
  MemoryPool<uint32_t[32]> containers_32;
  MemoryPool<uint32_t[64]> containers_64;
  MemoryPool<uint32_t[128]> containers_128;
  MemoryPool<uint32_t[256]> containers_256;
  MemoryPool<uint32_t[512]> containers_512;
  MemoryPool<uint32_t[1024]> containers_1024;


  static MemoryManager &instance();



};


#endif //RDFCACHEK2_MEMORYMANAGER_HPP
