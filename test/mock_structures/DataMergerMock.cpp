//
// Created by cristobal on 9/7/21.
//

#include "DataMergerMock.hpp"

namespace k2cache {
DataMergerMock::DataMergerMock(K2TreeConfig config) : config(config) {}

void DataMergerMock::merge_add_tree(unsigned long predicate_id,
                                    K2TreeMixed &k2tree) {
  auto it = trees.find(predicate_id);
  K2TreeMixed *tree_ptr;
  if (it == trees.end()) {
    auto new_tree = std::make_unique<K2TreeMixed>(config);
    tree_ptr = new_tree.get();
    trees[predicate_id] = std::move(new_tree);
  } else {
    tree_ptr = it->second.get();
  }

  auto scanner = k2tree.create_full_scanner();
  while (scanner->has_next()) {
    auto point = scanner->next();
    tree_ptr->insert(point.first, point.second);
  }
}

void DataMergerMock::merge_delete_tree(unsigned long predicate_id,
                                       K2TreeMixed &k2tree) {
  auto it = trees.find(predicate_id);
  K2TreeMixed *tree_ptr;
  if (it == trees.end()) {
    auto new_tree = std::make_unique<K2TreeMixed>(config);
    tree_ptr = new_tree.get();
    trees[predicate_id] = std::move(new_tree);
  } else {
    tree_ptr = it->second.get();
  }

  auto scanner = k2tree.create_full_scanner();
  while (scanner->has_next()) {
    auto point = scanner->next();
    tree_ptr->remove(point.first, point.second);
  }
}

void DataMergerMock::drop() {
  trees = std::unordered_map<unsigned long, std::unique_ptr<K2TreeMixed>>();
}

void DataMergerMock::merge_update(std::vector<K2TreeUpdates> &updates) {
  for (auto &update : updates) {
    if (update.k2tree_add)
      merge_add_tree(update.predicate_id, *update.k2tree_add);
    if (update.k2tree_del)
      merge_delete_tree(update.predicate_id, *update.k2tree_del);
  }
}
} // namespace k2cache