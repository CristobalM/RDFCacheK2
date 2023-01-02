//
// Created by cristobal on 03-10-22.
//

#ifndef RDFCACHEK2_MOCKFETCHER_HPP
#define RDFCACHEK2_MOCKFETCHER_HPP

#include <unordered_map>

#include "manager/K2TreeFetcher.hpp"

namespace k2cache{
class MockFetcher : public K2TreeFetcher {
  std::unordered_map<uint64_t, std::unique_ptr<K2TreeMixed>> data;
public:
  MockFetcher();
  PredicateFetchResult fetch_k2tree(uint64_t predicate_index) override;
  PredicateFetchResult
  fetch_k2tree_if_loaded(uint64_t predicate_index) override;
};

}

#endif // RDFCACHEK2_MOCKFETCHER_HPP
