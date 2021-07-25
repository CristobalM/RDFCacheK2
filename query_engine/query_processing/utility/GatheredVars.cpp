//
// Created by cristobal on 25-07-21.
//

#include "GatheredVars.hpp"
#include <algorithm>
GatheredVars::GatheredVars(std::set<unsigned long> &&table_vars,
                           std::set<unsigned long> &&ref_vars)
    : table_vars(std::move(table_vars)), ref_vars(std::move(ref_vars)) {
  std::set_union(this->table_vars.begin(), this->table_vars.end(),
                 this->ref_vars.begin(), this->ref_vars.end(),
                 std::inserter(all_vars, all_vars.begin()));
}
