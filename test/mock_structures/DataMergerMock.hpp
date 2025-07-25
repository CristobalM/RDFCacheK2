//
// Created by cristobal on 9/7/21.
//

#ifndef RDFCACHEK2_DATAMERGERMOCK_HPP
#define RDFCACHEK2_DATAMERGERMOCK_HPP

#include <memory>
#include <unordered_map>
#include <vector>

#include "k2tree/K2TreeMixed.hpp"
#include "updating/DataMerger.hpp"

namespace k2cache {
struct DataMergerMock : public DataMerger {

  K2TreeConfig config;
  explicit DataMergerMock(K2TreeConfig config);

  std::unordered_map<uint64_t, std::unique_ptr<K2TreeMixed>> trees;

  void merge_add_tree(uint64_t predicate_id, K2TreeMixed &k2tree) override;
  void merge_delete_tree(uint64_t predicate_id,
                         K2TreeMixed &k2tree) override;

  void drop();
  void merge_update(std::vector<K2TreeUpdates> &updates) override;
};
} // namespace k2cache

#endif // RDFCACHEK2_DATAMERGERMOCK_HPP
