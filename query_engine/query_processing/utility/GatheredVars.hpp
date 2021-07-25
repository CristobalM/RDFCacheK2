//
// Created by cristobal on 25-07-21.
//

#ifndef RDFCACHEK2_GATHEREDVARS_HPP
#define RDFCACHEK2_GATHEREDVARS_HPP

#include <set>
class GatheredVars {
public:
  std::set<unsigned long> table_vars;
  std::set<unsigned long> ref_vars;
  std::set<unsigned long> all_vars;
  GatheredVars(std::set<unsigned long> &&table_vars,
               std::set<unsigned long> &&ref_vars);
};

#endif // RDFCACHEK2_GATHEREDVARS_HPP
