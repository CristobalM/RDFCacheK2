//
// Created by cristobal on 8/14/21.
//

#ifndef RDFCACHEK2_I_REPLACEMENTPRIORITYQUEUE_HPP
#define RDFCACHEK2_I_REPLACEMENTPRIORITYQUEUE_HPP

#include "I_PQTraverse.hpp"
#include <memory>
struct I_ReplacementPriorityQueue {
  virtual ~I_ReplacementPriorityQueue() = default;
  virtual bool empty() = 0;
  virtual std::unique_ptr<I_PQTraverse> pq_traverse() = 0;
  virtual size_t get_key_size(unsigned long key) = 0;
};
#endif // RDFCACHEK2_I_REPLACEMENTPRIORITYQUEUE_HPP
