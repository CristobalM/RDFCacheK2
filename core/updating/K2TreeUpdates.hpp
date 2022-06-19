//
// Created by cristobal on 06-09-21.
//

#ifndef RDFCACHEK2_K2TREEUPDATES_HPP
#define RDFCACHEK2_K2TREEUPDATES_HPP

#include "k2tree/K2TreeMixed.hpp"

struct K2TreeUpdates {
  enum UPDATE_KIND { INSERT_UPDATE = 0, DELETE_UPDATE, BOTH_UPDATE, NO_UPDATE };

  unsigned long predicate_id;
  K2TreeMixed *k2tree_add;
  K2TreeMixed *k2tree_del;
  K2TreeUpdates(unsigned long predicate_id, K2TreeMixed *k2tree_add,
                K2TreeMixed *k2tree_del);
  UPDATE_KIND get_kind() const;
};

#endif // RDFCACHEK2_K2TREEUPDATES_HPP
