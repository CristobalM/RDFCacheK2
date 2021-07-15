#include "VarIndexManager.hpp"

VarIndexManager::VarIndexManager() : current_index(1) {}

unsigned long
VarIndexManager::assign_index_if_not_found(const std::string &var_name) {
  if (var_indexes.find(var_name) == var_indexes.end()) {
    auto result = current_index++;
    var_indexes[var_name] = result;
    return result;
  }
  return var_indexes[var_name];
}

std::unordered_map<unsigned long, std::string>
VarIndexManager::reverse() const {
  std::unordered_map<unsigned long, std::string> result;
  for (auto &item : var_indexes) {
    result[item.second] = item.first;
  }
  return result;
}
