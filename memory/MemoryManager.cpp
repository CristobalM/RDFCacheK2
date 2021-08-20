//
// Created by Cristobal Miranda, 2020
//

extern "C" {
#include "block.h"
#include "definitions.h"
#include "k2node.h"
#include "memalloc.h"
}

#include "MemoryManager.hpp"

MemoryManager::MemoryManager() : m(std::make_unique<std::mutex>()) {}

MemoryManager &MemoryManager::instance() { return _instance; }
MemorySegment *MemoryManager::new_memory_segment(size_t size) {
  std::lock_guard lg(*m);
  auto next_segment = std::make_unique<MemorySegment>(size);
  auto *ptr = next_segment.get();
  auto *first_address = ptr->get_first_address();
  memory_segments[first_address] = std::move(next_segment);
  reverse_mem_segments_map[ptr] = first_address;
  return ptr;
}
bool MemoryManager::exists(void *ptr) {
  std::lock_guard lg(*m);
  auto it = memory_segments.lower_bound(ptr);
  return it != memory_segments.end();
}
MemorySegment *MemoryManager::find_segment(void *ptr) {
  std::lock_guard lg(*m);
  auto it = memory_segments.lower_bound(ptr);
  if (it == memory_segments.end())
    return nullptr;
  auto *mem_segment = it->second.get();
  if (!mem_segment->has(ptr)) {
    return nullptr;
  }
  return mem_segment;
}
void MemoryManager::free_segment(MemorySegment *memory_segment) {
  auto it = reverse_mem_segments_map.find(memory_segment);
  if (it == reverse_mem_segments_map.end())
    return;
  auto it_ms = memory_segments.find(it->second);

  memory_segments.erase(it_ms);
  reverse_mem_segments_map.erase(it);
}
MemoryManager::MemoryManager(MemoryManager &&other) noexcept
    : m(std::move(other.m)) {}
MemoryManager &MemoryManager::operator=(MemoryManager &&other) noexcept {
  this->m = std::move(other.m);
  return *this;
}

struct block *k2tree_alloc_block(void) {
  return new struct block;
}

uint32_t *k2tree_alloc_u32array(int size) { return new uint32_t[size](); }

int k2tree_free_block(struct block *block) {
  auto *segment = MemoryManager::instance().find_segment((void *)block);
  if (segment)
    segment->free_memory((void *)block);
  else
    delete block;
  return 0;
}
int k2tree_free_u32array(uint32_t *data, int) {
  auto *segment = MemoryManager::instance().find_segment((void *)data);
  if (segment)
    segment->free_memory((void *)data);
  else
    delete[] data;
  return 0;
}

void *k2tree_alloc_preorders(int capacity) {
  return new NODES_BV_T[capacity]();
}
struct block *k2tree_alloc_blocks_array(int capacity) {
  return new struct block[capacity]();
}

void k2tree_free_preorders(void *preorders) {
  auto *segment = MemoryManager::instance().find_segment(preorders);
  if (segment)
    segment->free_memory((void *)preorders);
  else
    delete[]((NODES_BV_T *)preorders);
}
void k2tree_free_blocks_array(struct block *blocks_array) {
  auto *segment = MemoryManager::instance().find_segment((void *)blocks_array);
  if (segment)
    segment->free_memory((void *)blocks_array);
  else
    delete[] blocks_array;
}

struct k2node *k2tree_allocate_k2node(void) {
  return new struct k2node();
}

void k2tree_free_k2node(struct k2node *node) {
  auto *segment = MemoryManager::instance().find_segment((void *)node);
  if (segment)
    segment->free_memory((void *)node);
  else
    delete node;
}

MemoryManager MemoryManager::_instance;