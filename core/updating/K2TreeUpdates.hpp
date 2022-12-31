//
// Created by cristobal on 06-09-21.
//

#ifndef RDFCACHEK2_K2TREEUPDATES_HPP
#define RDFCACHEK2_K2TREEUPDATES_HPP

#include "k2tree/K2TreeMixed.hpp"

namespace k2cache {
struct K2TreeUpdates {
  enum UPDATE_KIND { INSERT_UPDATE = 0, DELETE_UPDATE, BOTH_UPDATE, NO_UPDATE };

  uint64_t predicate_id;
  K2TreeMixed *k2tree_add;
  K2TreeMixed *k2tree_del;
  K2TreeUpdates(uint64_t predicate_id, K2TreeMixed *k2tree_add,
                K2TreeMixed *k2tree_del);
  UPDATE_KIND get_kind() const;
};
} // namespace k2cache

#endif // RDFCACHEK2_K2TREEUPDATES_HPP
