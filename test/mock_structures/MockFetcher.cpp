//
// Created by cristobal on 03-10-22.
//

#include "MockFetcher.hpp"
namespace k2cache{

PredicateFetchResult MockFetcher::fetch_k2tree(uint64_t predicate_index) {
  return PredicateFetchResult(false, nullptr);
}
PredicateFetchResult
MockFetcher::fetch_k2tree_if_loaded(uint64_t predicate_index) {
  return PredicateFetchResult(false, nullptr);
}
}