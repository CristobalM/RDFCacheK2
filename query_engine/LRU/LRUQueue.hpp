#ifndef _LRU_QUEUE_HPP_
#define _LRU_QUEUE_HPP_

#include <list>
#include <unordered_map>

#include "LRUController.hpp"

struct LRUElement {
  unsigned long id;
  unsigned long size;
  LRUElement(unsigned long id, unsigned long size) : id(id), size(size) {}
};

class LRUQueue {
  std::list<LRUElement> lru_list;
  std::unordered_map<unsigned long, std::list<LRUElement>::iterator> lru_hm;

  LRUController &lru_controller;

  const unsigned long max_size_bytes;
  unsigned long space_used;

  unsigned long stats_erase_count;
  unsigned long stats_retrieval_count;

public:
  explicit LRUQueue(LRUController &lru_controller);

  void hit_element(unsigned long id, unsigned long element_size);

  unsigned long get_stats_erase_count();
  unsigned long get_stats_retrieval_count();

private:
  void require_space(unsigned long space_required);
};

#endif /* _LRU_QUEUE_HPP_ */
