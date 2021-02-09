#include <stdexcept>
#include <string>

#include "LRUQueue.hpp"

LRUQueue::LRUQueue(LRUController &lru_controller)
    : lru_controller(lru_controller),
      max_size_bytes(lru_controller.get_max_size_bytes()), space_used(0),
      stats_erase_count(0), stats_retrieval_count(0) {}

void LRUQueue::hit_element(unsigned long id, unsigned long element_size) {
  if (lru_hm.find(id) == lru_hm.end()) {
    require_space(element_size);
    lru_controller.retrieve_element(id);
    stats_retrieval_count++;
    auto it = lru_list.insert(lru_list.end(), LRUElement(id, element_size));
    space_used += element_size;
    lru_hm[id] = it;
  } else {
    auto it = lru_hm[id];
    if (it->size != element_size) {
      throw std::runtime_error(
          "LRUQueue::hit_element: Invalid sizes, size from queue differs from "
          "hit_element parameter, stored: " +
          std::to_string(it->size) +
          ", given: " + std::to_string(element_size));
    }
    lru_list.erase(it);
    auto new_it = lru_list.insert(lru_list.end(), LRUElement(id, element_size));
    space_used += element_size;
    lru_hm[id] = new_it;
  }
}

void LRUQueue::require_space(unsigned long space_required) {
  if (space_used + space_required <= max_size_bytes) {
    return;
  }

  while (space_used + space_required > max_size_bytes) {
    auto current_node = lru_list.front();

    lru_hm.erase(current_node.id);
    lru_list.pop_front();
    stats_erase_count++;

    // freed_space += lru_list.front().size;
    if (current_node.size > space_used)
      throw std::runtime_error(
          "LRUQueue::require_space failed because node of id " +
          std::to_string(current_node.size) +
          " has size bigger than space_used");
    lru_controller.discard_element(current_node.id);
    space_used -= current_node.size;
  }
}

unsigned long LRUQueue::get_stats_erase_count(){
  return stats_erase_count;
}
unsigned long LRUQueue::get_stats_retrieval_count(){
  return stats_retrieval_count;
}
