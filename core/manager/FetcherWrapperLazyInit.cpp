//
// Created by cristobal on 22-09-22.
//

#include "FetcherWrapperLazyInit.hpp"
#include "exceptions.hpp"
namespace k2cache {
PredicateFetchResult
FetcherWrapperLazyInit::fetch_k2tree(uint64_t predicate_index) {
  no_ref_check();
  return ref->fetch_k2tree(predicate_index);
}
PredicateFetchResult
FetcherWrapperLazyInit::fetch_k2tree_if_loaded(uint64_t predicate_index) {
  no_ref_check();
  return ref->fetch_k2tree_if_loaded(predicate_index);
}
void FetcherWrapperLazyInit::set_ref(K2TreeFetcher *r) { ref = r; }

void FetcherWrapperLazyInit::no_ref_check() {
  if (!ref) {
    throw NoRefException("on FetcherWrapperLazyInit");
  }
}

} // namespace k2cache
