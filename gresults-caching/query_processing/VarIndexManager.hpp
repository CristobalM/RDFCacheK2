#ifndef _VAR_INDEX_MANAGER_HPP_
#define _VAR_INDEX_MANAGER_HPP_
#include <string>
#include <unordered_map>

struct VarIndexManager {
  std::unordered_map<std::string, unsigned long> var_indexes;
  unsigned long current_index;

  VarIndexManager();
  void assign_index_if_not_found(const std::string &var_name);
  std::unordered_map<unsigned long, std::string> reverse();
};

#endif /* _VAR_INDEX_MANAGER_HPP_ */
