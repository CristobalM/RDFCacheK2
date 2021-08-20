//
// Created by cristobal on 20-08-21.
//

#include "MemorySegment.hpp"
MemorySegment::MemorySegment(size_t size_bytes)
    : data(std::make_unique<uint8_t[]>(size_bytes)), size_bytes(size_bytes),
      byte_position(0) {}
void *MemorySegment::require_bytes(size_t needed_bytes) {
  if (needed_bytes + byte_position > size_bytes)
    return nullptr;
  void *result = data.get() + byte_position;
  byte_position += needed_bytes;
  return result;
}
bool MemorySegment::has(void *ptr) {
  return ptr >= data.get() && ptr < data.get() + size_bytes;
}
void MemorySegment::free_memory(void *) {
  // do nothing for now
}
void *MemorySegment::get_first_address() { return data.get(); }
