//
// Created by cristobal on 7/21/21.
//

#ifndef RDFCACHEK2_VARSCOLLECTION_HPP
#define RDFCACHEK2_VARSCOLLECTION_HPP

#include "VarIndexManager.hpp"
#include <query_processing/utility/GatheredVars.hpp>
#include <set>
class VarsCollection {
  int position;

  std::set<unsigned long> all_vars;
  std::set<unsigned long> table_vars;
  std::set<unsigned long> reference_vars;

public:
  std::set<unsigned long> &get_reference_vars();
  std::set<unsigned long> &get_table_vars();
  std::set<unsigned long> &get_all_vars();
  int get_position() const;
  const std::set<unsigned long> &get_table_vars_c() const;
  VarsCollection(GatheredVars &&gathered_vars, int position);
};

#endif // RDFCACHEK2_VARSCOLLECTION_HPP
