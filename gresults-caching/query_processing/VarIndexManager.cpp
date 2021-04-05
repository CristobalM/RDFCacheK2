#include "VarIndexManager.hpp"

VarIndexManager::VarIndexManager() : current_index(0) {}

void VarIndexManager::assign_index_if_not_found(const std::string &var_name) {
  if (var_indexes.find(var_name) == var_indexes.end()) {
    var_indexes[var_name] = current_index++;
  }
}

std::unordered_map<unsigned long, std::string> VarIndexManager::reverse() {
  std::unordered_map<unsigned long, std::string> result;
  for (auto &item : var_indexes) {
    result[item.second] = item.first;
  }
  return result;
}
