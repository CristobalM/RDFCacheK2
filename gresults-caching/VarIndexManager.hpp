#ifndef _VAR_INDEX_MANAGER_HPP_
#define _VAR_INDEX_MANAGER_HPP_
#include <unordered_map>
#include <string>

struct VarIndexManager {
  std::unordered_map<std::string, unsigned long> var_indexes;
  unsigned long current_index;
  VarIndexManager() : current_index(0) {}

  void assign_index_if_not_found(const std::string &var_name) {
    if (var_indexes.find(var_name) == var_indexes.end()) {
      var_indexes[var_name] = current_index++;
    }
  }

  std::unordered_map<unsigned long, std::string> reverse(){
    std::unordered_map<unsigned long, std::string> result;
    for(auto &item : var_indexes){
      result[item.second] = item.first;
    }
    return result;
  }
};

#endif /* _VAR_INDEX_MANAGER_HPP_ */
