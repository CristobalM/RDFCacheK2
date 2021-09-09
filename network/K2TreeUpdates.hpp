//
// Created by cristobal on 06-09-21.
//

#ifndef RDFCACHEK2_K2TREEUPDATES_HPP
#define RDFCACHEK2_K2TREEUPDATES_HPP

#include <K2TreeMixed.hpp>
struct K2TreeUpdates {
  unsigned long predicate_id;
  K2TreeMixed *k2tree_add;
  K2TreeMixed *k2tree_del;
  K2TreeUpdates(unsigned long predicate_id, K2TreeMixed *k2tree_add,
                K2TreeMixed *k2tree_del);
};

#endif // RDFCACHEK2_K2TREEUPDATES_HPP
