//
// Created by cristobal on 21-05-22.
//

#include "InMemoryTreeMerger.hpp"

void InMemoryTreeMerger::merge_add_tree(unsigned long, K2TreeMixed &k2tree) {
  merge_op(k2tree, [](K2TreeBulkOp &op, unsigned long col, unsigned long row) {
    op.insert(col, row);
  });
}
void InMemoryTreeMerger::merge_delete_tree(unsigned long, K2TreeMixed &k2tree) {
  merge_op(k2tree, [](K2TreeBulkOp &op, unsigned long col, unsigned long row) {
    op.remove(col, row);
  });
}
void InMemoryTreeMerger::merge_update(std::vector<K2TreeUpdates> &updates) {
  (void)updates;
}
void InMemoryTreeMerger::merge_op(
    K2TreeMixed &k2tree,
    const std::function<void(K2TreeBulkOp &, unsigned long, unsigned long)>
        &op_fun) {
  K2TreeBulkOp op(*current_tree);
  auto scanner = k2tree.create_full_scanner();
  while (scanner->has_next()) {
    auto pair = scanner->next();
    op_fun(op, pair.first, pair.second);
  }
}
