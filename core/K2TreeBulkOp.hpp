//
// Created by cristobal on 03-08-21.
//

#ifndef RDFCACHEK2_K2TREEBULKOP_HPP
#define RDFCACHEK2_K2TREEBULKOP_HPP

#include "K2TreeMixed.hpp"
class K2TreeBulkOp {
  K2TreeMixed &k2tree;
  K2QStateWrapper stw;

public:
  explicit K2TreeBulkOp(K2TreeMixed &k2tree);
  void insert(unsigned long col, unsigned long row);
  void remove(unsigned long col, unsigned long row);
  bool has(unsigned long col, unsigned long row);
  K2QStateWrapper &get_stw();
  K2TreeMixed &get_tree();
};

#endif // RDFCACHEK2_K2TREEBULKOP_HPP
