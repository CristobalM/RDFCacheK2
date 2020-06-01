//
// Created by Cristobal Miranda, 2020
//

extern "C" {
#include <bitvector.h>
#include <vector.h>

#include <block.h>
#include <block_frontier.h>
#include <block_topology.h>

#include <memalloc.h>

#include <definitions.h>
}

#include "MemoryManager.hpp"

#include <cmath>
#include <string>

struct block *k2tree_alloc_block(void) {
  auto *out = MemoryManager::instance().blocks.request_memory();
  out->block_index = MemoryManager::instance().new_block_index();
  return out;
}

struct block_topology *k2tree_alloc_block_topology(void) {
  return MemoryManager::instance().block_topologies.request_memory();
}

struct block_frontier *k2tree_alloc_block_frontier(void) {
  return MemoryManager::instance().block_frontiers.request_memory();
}

struct bitvector *k2tree_alloc_bitvector(void) {
  return MemoryManager::instance().bitvectors.request_memory();
}

struct vector *k2tree_alloc_vector(void) {
  return MemoryManager::instance().vectors.request_memory();
}

struct u32array_alloc k2tree_alloc_u32array(int size) {
  if (size > 1024) {
    throw std::runtime_error(
        "Blocks with size higher than 1024 are not allowed");
  }
  size_t container_sz = 1UL
                        << (unsigned long)std::ceil(std::log2((double)size));
  struct u32array_alloc out {};

  if (container_sz <= 32) {
    out.data =
        (uint32_t *)MemoryManager::instance().containers_32.request_memory();
    out.size = 32;
    return out;
  }

  switch (container_sz) {
  case 64:
    out.data =
        (uint32_t *)MemoryManager::instance().containers_64.request_memory();
    out.size = 64;
    break;
  case 128:
    out.data =
        (uint32_t *)MemoryManager::instance().containers_128.request_memory();
    out.size = 128;
    break;
  case 256:
    out.data =
        (uint32_t *)MemoryManager::instance().containers_256.request_memory();
    out.size = 256;
    break;
  case 512:
    out.data =
        (uint32_t *)MemoryManager::instance().containers_512.request_memory();
    out.size = 512;
    break;
  case 1024:
    out.data =
        (uint32_t *)MemoryManager::instance().containers_1024.request_memory();
    out.size = 1024;
    break;
  default:
    throw std::runtime_error("Invalid container_sz: " +
                             std::to_string(container_sz));
  }

  return out;
}

int k2tree_free_block(struct block *b) {
  MemoryManager::instance().blocks.free_memory(b);
  return 0;
}

int k2tree_free_block_topology(struct block_topology *bt) {
  MemoryManager::instance().block_topologies.free_memory(bt);
  return 0;
}

int k2tree_free_block_frontier(struct block_frontier *bf) {
  MemoryManager::instance().block_frontiers.free_memory(bf);
  return 0;
}

int k2tree_free_bitvector(struct bitvector *bv) {
  MemoryManager::instance().bitvectors.free_memory(bv);
  return 0;
}

int k2tree_free_vector(struct vector *v) {
  MemoryManager::instance().vectors.free_memory(v);
  return 0;
}

int k2tree_free_u32array(struct u32array_alloc alloc) {
  using b32_t = uint32_t[32];
  using b64_t = uint32_t[64];
  using b128_t = uint32_t[128];
  using b256_t = uint32_t[256];
  using b512_t = uint32_t[512];
  using b1024_t = uint32_t[1024];
  if (alloc.size <= 32) {
    MemoryManager::instance().containers_32.free_memory(
        reinterpret_cast<b32_t *>(alloc.data));
    return 0;
  }
  switch (alloc.size) {
  case 64:
    MemoryManager::instance().containers_64.free_memory(
        reinterpret_cast<b64_t *>(alloc.data));
    break;
  case 128:
    MemoryManager::instance().containers_128.free_memory(
        reinterpret_cast<b128_t *>(alloc.data));
    break;
  case 256:
    MemoryManager::instance().containers_256.free_memory(
        reinterpret_cast<b256_t *>(alloc.data));
    break;
  case 512:
    MemoryManager::instance().containers_512.free_memory(
        reinterpret_cast<b512_t *>(alloc.data));
    break;
  case 1024:
    MemoryManager::instance().containers_1024.free_memory(
        reinterpret_cast<b1024_t *>(alloc.data));
    break;
  default:
    throw std::runtime_error("Invalid container_sz" +
                             std::to_string(alloc.size));
  }
  return 0;
}
