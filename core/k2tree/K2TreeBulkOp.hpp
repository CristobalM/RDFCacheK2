//
// Created by cristobal on 03-08-21.
//

#ifndef RDFCACHEK2_K2TREEBULKOP_HPP
#define RDFCACHEK2_K2TREEBULKOP_HPP

#include <cstdint>

#include "K2TreeMixed.hpp"

namespace k2cache {
class K2TreeBulkOp {
  K2TreeMixed &k2tree;
  K2QStateWrapper stw;

public:
  explicit K2TreeBulkOp(K2TreeMixed &k2tree);
  void insert(uint64_t col, uint64_t row);
  bool insert_was_inserted(uint64_t col, uint64_t row);
  void remove(uint64_t col, uint64_t row);
  bool has(uint64_t col, uint64_t row);
  K2QStateWrapper &get_stw();
  K2TreeMixed &get_tree();
};
} // namespace k2cache
#endif // RDFCACHEK2_K2TREEBULKOP_HPP
