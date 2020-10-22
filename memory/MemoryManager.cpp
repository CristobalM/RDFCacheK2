//
// Created by Cristobal Miranda, 2020
//

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

MemoryManager MemoryManager::_instance;