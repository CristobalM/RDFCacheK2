//
// Created by cristobal on 22-09-22.
//

#ifndef RDFCACHEK2_FETCHERWRAPPERLAZYINIT_HPP
#define RDFCACHEK2_FETCHERWRAPPERLAZYINIT_HPP
#include "K2TreeFetcher.hpp"
namespace k2cache {
class FetcherWrapperLazyInit : public K2TreeFetcher {
  K2TreeFetcher *ref;

public:
  void set_ref(K2TreeFetcher *r);
  PredicateFetchResult fetch_k2tree(uint64_t predicate_index) override;
  PredicateFetchResult
  fetch_k2tree_if_loaded(uint64_t predicate_index) override;
  void no_ref_check();
};
} // namespace k2cache

#endif // RDFCACHEK2_FETCHERWRAPPERLAZYINIT_HPP
