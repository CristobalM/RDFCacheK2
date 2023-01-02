//
// Created by cristobal on 06-09-21.
//

#ifndef RDFCACHEK2_DATAMERGER_HPP
#define RDFCACHEK2_DATAMERGER_HPP

#include "K2TreeUpdates.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include "updating/K2TreeUpdates.hpp"

namespace k2cache {
class DataMerger {
public:
  virtual ~DataMerger() = default;

  virtual void merge_add_tree(uint64_t predicate_id,
                              K2TreeMixed &k2tree) = 0;
  virtual void merge_delete_tree(uint64_t predicate_id,
                                 K2TreeMixed &k2tree) = 0;
  virtual void merge_update(std::vector<K2TreeUpdates> &updates) = 0;
};
} // namespace k2cache

#endif // RDFCACHEK2_DATAMERGER_HPP
