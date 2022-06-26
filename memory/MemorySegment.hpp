//
// Created by cristobal on 20-08-21.
//

#ifndef RDFCACHEK2_MEMORYSEGMENT_HPP
#define RDFCACHEK2_MEMORYSEGMENT_HPP

#include <cstddef>
#include <memory>
namespace k2cache {
class MemorySegment {
  std::unique_ptr<uint8_t[]> data;
  size_t size_bytes;
  size_t byte_position;

public:
  explicit MemorySegment(size_t size_bytes);
  void *require_bytes(size_t needed_bytes);
  bool has(void *ptr);
  void free_memory(void *ptr);
  void *get_first_address();
};
} // namespace k2cache
#endif // RDFCACHEK2_MEMORYSEGMENT_HPP
