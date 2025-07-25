//
// Created by cristobal on 03-08-21.
//

#include "K2TreeBulkOp.hpp"
namespace k2cache {
K2TreeBulkOp::K2TreeBulkOp(K2TreeMixed &k2tree)
    : k2tree(k2tree), stw(k2tree.create_k2qw()) {}
void K2TreeBulkOp::insert(uint64_t col, uint64_t row) {
  k2tree.insert(col, row, stw);
}

void K2TreeBulkOp::remove(uint64_t col, uint64_t row) {
  k2tree.remove(col, row, stw);
}
bool K2TreeBulkOp::has(uint64_t col, uint64_t row) {
  return k2tree.has(col, row, stw);
}

K2QStateWrapper &K2TreeBulkOp::get_stw() { return stw; }
K2TreeMixed &K2TreeBulkOp::get_tree() { return k2tree; }

bool K2TreeBulkOp::insert_was_inserted(uint64_t col, uint64_t row) {
  return k2tree.insert_was_inserted(col, row);
}
} // namespace k2cache
