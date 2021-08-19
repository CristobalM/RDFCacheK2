//
// Created by Cristobal Miranda, 2020
//

extern "C" {
#include "memalloc.h"
}

#include <cmath>
#include <sstream>

#include "MemoryManager.hpp"
#include "MemoryPool.hpp"

const size_t default_pool_block_sz = 512;

MemoryManager::MemoryManager()
    : current_index(0), blocks(649880), containers_32(1299760),
      containers_64(default_pool_block_sz),
      containers_128(default_pool_block_sz),
      containers_256(default_pool_block_sz),
      containers_512(default_pool_block_sz),
      containers_1024(default_pool_block_sz) {}

MemoryManager &MemoryManager::instance() { return _instance; }

template <class T>
void report_pool_memory_usage(std::stringstream &ss, MemoryPool<T> &mp,
                              const std::string &name = "") {
  if (name != "") {
    ss << "(" << name << ")";
  }
  ss << "  items occupied: " << mp.get_items_occupied()
     << ", bytes occupied: " << mp.get_bytes_occupied()
     << ", items allocated: " << mp.get_items_allocated()
     << ", bytes allocated: " << mp.get_bytes_allocated()
     << ", usage rate items: " << mp.get_usage_rate()
     << ", usage rate bytes: " << mp.get_bytes_usage_rate()
     << ", times allocated: " << mp.get_times_allocated() << "\n";
}

std::string MemoryManager::memory_usage() {
  std::stringstream ss;

  report_pool_memory_usage(ss, blocks, "blocks");
  report_pool_memory_usage(ss, containers_32, "containers_32");
  report_pool_memory_usage(ss, containers_64, "containers_64");
  report_pool_memory_usage(ss, containers_128, "containers_128");
  report_pool_memory_usage(ss, containers_256, "containers_128");
  report_pool_memory_usage(ss, containers_512, "containers_512");
  report_pool_memory_usage(ss, containers_1024, "containers_1024");

  size_t total_bytes_occupied =
      blocks.get_bytes_occupied() + containers_32.get_bytes_occupied() +
      containers_64.get_bytes_occupied() + containers_128.get_bytes_occupied() +
      containers_256.get_bytes_occupied() +
      containers_512.get_bytes_occupied() +
      containers_1024.get_bytes_occupied();

  size_t total_bytes_allocated = blocks.get_bytes_allocated() +
                                 containers_32.get_bytes_allocated() +
                                 containers_64.get_bytes_allocated() +
                                 containers_128.get_bytes_allocated() +
                                 containers_256.get_bytes_allocated() +
                                 containers_512.get_bytes_allocated() +
                                 containers_1024.get_bytes_allocated();

  float byte_usage_rate =
      (float)((float)total_bytes_occupied / (float)total_bytes_allocated);

  ss << "Total bytes occupied: " << total_bytes_occupied << "\n"
     << "Total bytes allocated: " << total_bytes_allocated << "\n"
     << "Byte usage rate: " << byte_usage_rate << "\n";

  return ss.str();
}
uint64_t MemoryManager::new_block_index() {
  std::lock_guard<std::mutex> lg(m);
  return current_index++;
}

struct block *k2tree_alloc_block(void) {
  return MemoryManager::instance().blocks.request_memory();
}

uint32_t *k2tree_alloc_u32array(int size) {
  long next_power_of2;
  if (size <= 32)
    next_power_of2 = 32;
  else
    next_power_of2 = 1 << (long)std::ceil(std::log2(size));
  auto &inst = MemoryManager::instance();
  if (next_power_of2 > 1024) {
    auto &other_containers = inst.other_containers;
    auto result_uptr = std::make_unique<uint32_t[]>(size);
    auto *result = result_uptr.get();
    other_containers[result] = std::move(result_uptr);
    return result;
  }

  switch (next_power_of2) {
  case 32:
    return (uint32_t *)inst.containers_32.request_memory();
  case 64:
    return (uint32_t *)inst.containers_64.request_memory();
  case 128:
    return (uint32_t *)inst.containers_128.request_memory();
  case 256:
    return (uint32_t *)inst.containers_256.request_memory();
  case 512:
    return (uint32_t *)inst.containers_512.request_memory();
  case 1024:
    return (uint32_t *)inst.containers_1024.request_memory();
  }

  return nullptr;
}

int k2tree_free_block(struct block *block) {
  MemoryManager::instance().blocks.free_memory(block);
  return 0;
}
int k2tree_free_u32array(uint32_t *data, int size) {
  long next_power_of2;
  if (size <= 32)
    next_power_of2 = 32;
  else
    next_power_of2 = (long)std::ceil(std::log2(size));
  auto &inst = MemoryManager::instance();
  if (next_power_of2 > 1024) {
    auto &other_containers = inst.other_containers;
    auto it = other_containers.find(data);
    if (it == other_containers.end())
      return 0;
    other_containers.erase(it);
    return 0;
  }

  switch (next_power_of2) {
  case 32:
    inst.containers_32.free_memory((uint32_t(*)[32])(data));
    break;
  case 64:
    inst.containers_64.free_memory((uint32_t(*)[64])data);
    break;
  case 128:
    inst.containers_128.free_memory((uint32_t(*)[128])data);
    break;
  case 256:
    inst.containers_256.free_memory((uint32_t(*)[256])data);
    break;
  case 512:
    inst.containers_512.free_memory((uint32_t(*)[512])data);
    break;
  case 1024:
    inst.containers_1024.free_memory((uint32_t(*)[1024])data);
    break;
  default:
    return 123123;
  }
  return 0;
}

MemoryManager MemoryManager::_instance;