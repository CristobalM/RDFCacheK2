//
// Created by cristobal on 7/21/21.
//
#include "VarsCollection.hpp"
#include <algorithm>

std::set<unsigned long> &VarsCollection::get_reference_vars() {
  return reference_vars;
}
std::set<unsigned long> &VarsCollection::get_table_vars() { return table_vars; }
std::set<unsigned long> &VarsCollection::get_all_vars() { return all_vars; }
int VarsCollection::get_position() const { return position; }
const std::set<unsigned long> &VarsCollection::get_table_vars_c() const {
  return table_vars;
}
VarsCollection::VarsCollection(GatheredVars &&gathered_vars, int position)
    : position(position), all_vars(std::move(gathered_vars.all_vars)),
      table_vars(std::move(gathered_vars.table_vars)),
      reference_vars(std::move(gathered_vars.ref_vars)) {}
