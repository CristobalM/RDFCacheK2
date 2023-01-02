//
// Created by cristobal on 22-09-22.
//

#ifndef RDFCACHEK2_PCMMERGERWRAPPER_HPP
#define RDFCACHEK2_PCMMERGERWRAPPER_HPP
#include "DataMerger.hpp"
namespace k2cache {
class PCMMergerWrapper : public DataMerger {
  DataMerger *ref{};

public:
  void set_ref(DataMerger *ref);
  void merge_add_tree(uint64_t predicate_id, K2TreeMixed &k2tree) override;
  void merge_delete_tree(uint64_t predicate_id,
                         K2TreeMixed &k2tree) override;
  void merge_update(std::vector<K2TreeUpdates> &updates) override;
  void no_ref_check();
};

} // namespace k2cache

#endif // RDFCACHEK2_PCMMERGERWRAPPER_HPP
