//
// Created by cristobal on 7/21/21.
//

#include "VarsCollection.hpp"
VarsCollection::VarsCollection(VarIndexManager &vim, int position)
    : vim(vim), position(position) {}
void VarsCollection::add_table_var(const std::string &var_str) {
  auto id = get_id(var_str);
  all_vars.insert(id);
  table_vars.insert(id);
  if (reference_vars.find(id) == reference_vars.end()) {
    reference_vars.erase(id);
  }
}
unsigned long VarsCollection::get_id(const std::string &var_str) {
  return vim.assign_index_if_not_found(var_str);
}
void VarsCollection::add_reference_var(const std::string &var_str) {
  auto id = get_id(var_str);
  all_vars.insert(id);
  if (table_vars.find(id) == table_vars.end())
    reference_vars.insert(id);
}
std::set<unsigned long> &VarsCollection::get_reference_vars() {
  return reference_vars;
}
std::set<unsigned long> &VarsCollection::get_table_vars() { return table_vars; }
std::set<unsigned long> &VarsCollection::get_all_vars() { return all_vars; }
int VarsCollection::get_position() const { return position; }
const std::set<unsigned long> &VarsCollection::get_table_vars_c() const {
  return table_vars;
}
