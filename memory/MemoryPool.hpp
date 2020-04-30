//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_MEMORYPOOL_HPP
#define RDFCACHEK2_MEMORYPOOL_HPP

#include <memory>
#include <queue>
#include <vector>
#include <stdexcept>

template <class T>
class MemoryPool {
  using buffer_t = T[];

  std::vector<std::unique_ptr<buffer_t>> buffers;
  std::priority_queue<size_t> item_queue;

  size_t pool_block_capacity;

public:
  explicit MemoryPool(size_t pool_block_capacity) : pool_block_capacity(pool_block_capacity){
    auto buffer = std::make_unique<buffer_t>(pool_block_capacity);
    buffers.push_back(buffer);
    item_queue.push(0);
  }

  T * request_memory(){
    ensure_enough_capacity();
    auto next_item_pos = item_queue.top();
    item_queue.pop();
    size_t buffer_index = next_item_pos / pool_block_capacity;
    size_t buffer_offset = next_item_pos % pool_block_capacity;
    auto result = buffers[buffer_index] + buffer_offset;

    if(item_queue.empty() || next_item_pos + 1 > item_queue.top()){
      item_queue.push(next_item_pos + 1);
    }

    return result;
  }

  void free_memory(T *ptr){
    size_t buffer_index = find_owner_buffer(ptr);
    auto *buffer = buffers[buffer_index].get();
    size_t offset = ptr - buffer;
    size_t item_index = offset + (buffer_index) * pool_block_capacity;
    item_queue.push(item_index);
  }

private:
  void ensure_enough_capacity(){
    if(item_queue.top() + 1 > pool_block_capacity * buffers.size()){
      /* The pool is out of space */
      auto buffer = std::make_unique<buffer_t>(pool_block_capacity);
      buffers.push_back(buffer);
    }
  }

  size_t find_owner_buffer(T *ptr){
    // TODO: binary search if needed
    for(size_t i = 0; i < buffers.size(); i++){
      T *last_in_buffer = buffers[i].get() + (pool_block_capacity - 1);
      T *first_in_buffer = buffers[i].get();
      if(first_in_buffer <= ptr && last_in_buffer >= ptr){
        return i;
      }
    }
    throw std::runtime_error("Couldn't find pointer in buffers");
  }

};


#endif //RDFCACHEK2_MEMORYPOOL_HPP
