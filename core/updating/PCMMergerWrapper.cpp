//
// Created by cristobal on 22-09-22.
//

#include "PCMMergerWrapper.hpp"
#include "exceptions.hpp"

namespace k2cache {

void PCMMergerWrapper::merge_add_tree(uint64_t predicate_id,
                                      K2TreeMixed &k2tree) {
  no_ref_check();
  ref->merge_add_tree(predicate_id, k2tree);
}
void PCMMergerWrapper::merge_delete_tree(uint64_t predicate_id,
                                         K2TreeMixed &k2tree) {
  no_ref_check();
  ref->merge_delete_tree(predicate_id, k2tree);
}
void PCMMergerWrapper::merge_update(std::vector<K2TreeUpdates> &updates) {
  no_ref_check();
  ref->merge_update(updates);
}
void PCMMergerWrapper::set_ref(DataMerger *r) { ref = r; }
void PCMMergerWrapper::no_ref_check() {
  if (!ref) {
    throw NoRefException("on PCMMergerWrapper");
  }
}

} // namespace k2cache