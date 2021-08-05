//
// Created by cristobal on 04-08-21.
//

#ifndef RDFCACHEK2_CACHEREPLACEMENT_HPP
#define RDFCACHEK2_CACHEREPLACEMENT_HPP

#include "I_CacheReplacement.hpp"
#include <I_DataManager.hpp>
#include <cstddef>
#include <map>
#include <mutex>
#include <set>
#include <unordered_map>
#include <vector>

template <class CRStrategy> class CacheReplacement : public I_CacheReplacement {

  struct StrategyWrapper {
    CRStrategy &strategy;
    explicit StrategyWrapper(CRStrategy &strategy) : strategy(strategy) {}
    bool operator()(unsigned long lhs, unsigned long rhs) const {
      return strategy.operator()(lhs, rhs);
    }
  };

  CRStrategy strategy;
  std::set<unsigned long, StrategyWrapper> priority_set;

  size_t max_size_allowed;
  size_t size_used;

  std::unordered_map<unsigned long, size_t> space_map;

  I_DataManager *data_manager;

  std::map<unsigned long, int> in_use;

  std::mutex &replacement_mutex;

public:
  CacheReplacement(size_t max_size_allowed, I_DataManager *data_manager,
                   std::mutex &replacement_mutex);

  // returns true if the key can be allocated else returns false
  bool hit_key(unsigned long key, size_t space_required) override;

  void mark_using(unsigned long key) override;
  void mark_ready(unsigned long key) override;
};

#endif // RDFCACHEK2_CACHEREPLACEMENT_HPP
