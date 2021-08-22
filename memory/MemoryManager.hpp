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

#include <list>
#include <vector>

#include "MemorySegment.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>

class MemoryManager {

  std::unique_ptr<std::mutex> m;

protected:
  MemoryManager();

  MemoryManager(MemoryManager &&other) noexcept;
  MemoryManager &operator=(MemoryManager &&other) noexcept;

  static MemoryManager _instance;

  struct Comp {
    bool operator()(void *lhs, void *rhs) const { return lhs > rhs; }
  };

  std::map<void *, std::unique_ptr<MemorySegment>, Comp> memory_segments;
  std::map<MemorySegment *, void *> reverse_mem_segments_map;

public:
  static MemoryManager &instance();

  MemorySegment *new_memory_segment(size_t size);

  bool exists(void *ptr);
  MemorySegment *find_segment(void *ptr);

  void free_segment(MemorySegment *memory_segment);
};

#endif // RDFCACHEK2_MEMORYMANAGER_HPP
