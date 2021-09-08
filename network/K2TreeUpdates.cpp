//
// Created by cristobal on 06-09-21.
//

#include "K2TreeUpdates.hpp"
K2TreeUpdates::K2TreeUpdates(unsigned long predicate_id,
                             K2TreeMixed *k2tree_add, K2TreeMixed *k2tree_del)

    : predicate_id(predicate_id), k2tree_add(k2tree_add),
      k2tree_del(k2tree_del) {}
