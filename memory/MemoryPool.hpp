//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_MEMORYPOOL_HPP
#define RDFCACHEK2_MEMORYPOOL_HPP

#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <vector>

template <class T> class MemoryPool {
  using buffer_t = T[];

  std::vector<std::unique_ptr<buffer_t>> buffers;
  std::priority_queue<size_t, std::vector<size_t>, std::greater<>> item_queue;

  size_t pool_block_capacity;

  size_t items_occupied;
  size_t allocation_times;

  std::mutex m;

  using lg_t = std::lock_guard<std::mutex>;

public:
  explicit MemoryPool(size_t pool_block_capacity)
      : pool_block_capacity(pool_block_capacity), items_occupied(0),
        allocation_times(0) {
    auto buffer = std::make_unique<buffer_t>(pool_block_capacity);
    buffers.push_back(std::move(buffer));
    item_queue.push(0);
  }

  T *request_memory() {
    lg_t lg(m);
    ensure_enough_capacity();
    auto next_item_pos = item_queue.top();
    item_queue.pop();
    size_t buffer_index = next_item_pos / pool_block_capacity;
    size_t buffer_offset = next_item_pos % pool_block_capacity;
    T *result =
        reinterpret_cast<T *>(buffers[buffer_index].get()) + buffer_offset;

    if (next_item_pos + 1 > item_queue.top()) {
      item_queue.push(next_item_pos + 1);
    }

    items_occupied++;

    return result;
  }

  void free_memory(T *ptr) {
    lg_t lg(m);
    size_t buffer_index = find_owner_buffer(ptr);
    auto *buffer = reinterpret_cast<T *>(buffers[buffer_index].get());
    size_t offset = ptr - buffer;
    size_t item_index = offset + (buffer_index)*pool_block_capacity;
    item_queue.push(item_index);
    items_occupied--;
  }

  void free_all_memory() {
    lg_t lg(m);
    buffers.clear();
    items_occupied = 0;
    allocation_times = 0;
  }

  size_t get_bytes_occupied() {
    lg_t lg(m);
    return sizeof(T) * items_occupied;
  }

  size_t get_items_occupied() {
    lg_t lg(m);
    return items_occupied;
  }

  size_t get_bytes_allocated() {
    lg_t lg(m);
    return sizeof(T) * buffers.size() * pool_block_capacity;
  }

  size_t get_items_allocated() {
    lg_t lg(m);
    return buffers.size() * pool_block_capacity;
  }

  float get_usage_rate() {
    lg_t lg(m);

    return (float)get_items_occupied() / (float)get_items_allocated();
  }

  float get_bytes_usage_rate() {
    lg_t lg(m);

    return (float)get_bytes_occupied() / (float)get_bytes_allocated();
  }

  size_t get_times_allocated() {
    lg_t lg(m);
    return allocation_times;
  }

private:
  void ensure_enough_capacity() {
    if (item_queue.top() + 1 > pool_block_capacity * buffers.size()) {
      /* The pool is out of space */
      auto buffer = std::make_unique<buffer_t>(pool_block_capacity);
      buffers.push_back(std::move(buffer));
      allocation_times++;
    }
  }

  size_t find_owner_buffer(T *ptr) {
    // TODO: binary search if needed
    for (size_t i = 0; i < buffers.size(); i++) {
      T *last_in_buffer =
          reinterpret_cast<T *>(buffers[i].get()) + (pool_block_capacity - 1);
      T *first_in_buffer = reinterpret_cast<T *>(buffers[i].get());
      if (first_in_buffer <= ptr && last_in_buffer >= ptr) {
        return i;
      }
    }
    throw std::runtime_error("Couldn't find pointer in buffers");
  }
};

#endif // RDFCACHEK2_MEMORYPOOL_HPP
