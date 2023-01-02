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
    bool operator()(uint64_t lhs, uint64_t rhs) const;
  };

  using pq_t = std::set<uint64_t, StrategyWrapper>;
  using space_map_t = std::unordered_map<uint64_t, size_t>;

  class CRPQTraverse : public I_PQTraverse {
    pq_t &pq;
    typename pq_t::iterator it;

  public:
    explicit CRPQTraverse(pq_t &pq);
    bool has_next() override;
    uint64_t next_key() override;
  };

  class CRPriorityQueue : public I_ReplacementPriorityQueue {
    pq_t &priority_queue;
    space_map_t &space_map;

  public:
    CRPriorityQueue(pq_t &replacement_priority_queue, space_map_t &space_map);
    bool empty() override;
    std::unique_ptr<I_PQTraverse> pq_traverse() override;
    size_t get_key_size(uint64_t key) override;
  };

  CRStrategy strategy;
  pq_t priority_set;
  space_map_t space_map;

  CRPriorityQueue cr_priority_queue;

  size_t max_size_allowed;
  size_t size_used;

  DataManager *data_manager;

  std::map<uint64_t, int> in_use;

  std::mutex m;

public:
  CacheReplacement(size_t max_size_allowed, DataManager *data_manager);

  // returns true if the key can be allocated else returns false
  bool hit_key(uint64_t key, size_t space_required) override;

  void mark_using(uint64_t key) override;
  void mark_ready(uint64_t key) override;
  bool is_using(uint64_t key);
  std::mutex &get_replacement_mutex() override;
};
} // namespace k2cache

#endif // RDFCACHEK2_CACHEREPLACEMENT_HPP
