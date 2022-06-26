//
// Created by cristobal on 04-08-21.
//

#ifndef RDFCACHEK2_CACHEREPLACEMENT_HPP
#define RDFCACHEK2_CACHEREPLACEMENT_HPP

#include "I_CacheReplacement.hpp"
#include "I_PQTraverse.hpp"
#include "I_ReplacementPriorityQueue.hpp"
#include "manager/DataManager.hpp"
#include <cstddef>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <unordered_map>
#include <vector>

namespace k2cache {
template <class CRStrategy> class CacheReplacement : public I_CacheReplacement {
  struct StrategyWrapper {
    CRStrategy &strategy;
    explicit StrategyWrapper(CRStrategy &strategy);
    bool operator()(unsigned long lhs, unsigned long rhs) const;
  };

  using pq_t = std::set<unsigned long, StrategyWrapper>;
  using space_map_t = std::unordered_map<unsigned long, size_t>;

  class CRPQTraverse : public I_PQTraverse {
    pq_t &pq;
    typename pq_t::iterator it;

  public:
    explicit CRPQTraverse(pq_t &pq);
    bool has_next() override;
    unsigned long next_key() override;
  };

  class CRPriorityQueue : public I_ReplacementPriorityQueue {
    pq_t &priority_queue;
    space_map_t &space_map;

  public:
    CRPriorityQueue(pq_t &replacement_priority_queue, space_map_t &space_map);
    bool empty() override;
    std::unique_ptr<I_PQTraverse> pq_traverse() override;
    size_t get_key_size(unsigned long key) override;
  };

  CRStrategy strategy;
  pq_t priority_set;
  space_map_t space_map;

  CRPriorityQueue cr_priority_queue;

  size_t max_size_allowed;
  size_t size_used;

  DataManager *data_manager;

  std::map<unsigned long, int> in_use;

  std::mutex m;

public:
  CacheReplacement(size_t max_size_allowed, DataManager *data_manager);

  // returns true if the key can be allocated else returns false
  bool hit_key(unsigned long key, size_t space_required) override;

  void mark_using(unsigned long key) override;
  void mark_ready(unsigned long key) override;
  bool is_using(unsigned long key);
  std::mutex &get_replacement_mutex() override;
};
} // namespace k2cache

#endif // RDFCACHEK2_CACHEREPLACEMENT_HPP
