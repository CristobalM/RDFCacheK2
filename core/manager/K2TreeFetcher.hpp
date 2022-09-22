//
// Created by cristobal on 22-09-22.
//

#ifndef RDFCACHEK2_K2TREEFETCHER_HPP
#define RDFCACHEK2_K2TREEFETCHER_HPP
#include "PredicateFetchResult.hpp"
namespace k2cache {
class K2TreeFetcher {
public:
  virtual ~K2TreeFetcher() noexcept = default;
  virtual PredicateFetchResult fetch_k2tree(uint64_t predicate_index) = 0;
  virtual PredicateFetchResult
  fetch_k2tree_if_loaded(uint64_t predicate_index) = 0;
};
} // namespace k2cache
#endif // RDFCACHEK2_K2TREEFETCHER_HPP
