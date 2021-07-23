//
// Created by cristobal on 7/21/21.
//

#ifndef RDFCACHEK2_VARSCOLLECTION_HPP
#define RDFCACHEK2_VARSCOLLECTION_HPP

#include "VarIndexManager.hpp"
#include <set>
class VarsCollection {
  VarIndexManager &vim;
  int position;

  std::set<unsigned long> all_vars;
  std::set<unsigned long> table_vars;
  std::set<unsigned long> reference_vars;

public:
  VarsCollection(VarIndexManager &vim, int position);
  void add_table_var(const std::string &var_str);
  unsigned long get_id(const std::string &var_str);
  void add_reference_var(const std::string &var_str);
  std::set<unsigned long> &get_reference_vars();
  std::set<unsigned long> &get_table_vars();
  std::set<unsigned long> &get_all_vars();
  int get_position() const;
  const std::set<unsigned long> &get_table_vars_c() const;
};

#endif // RDFCACHEK2_VARSCOLLECTION_HPP
