#ifndef _VAR_INDEX_MANAGER_HPP_
#define _VAR_INDEX_MANAGER_HPP_
#include <string>
#include <unordered_map>

namespace k2cache {

struct VarIndexManager {
  std::unordered_map<std::string, unsigned long> var_indexes;
  unsigned long current_index;

  VarIndexManager();
  unsigned long assign_index_if_not_found(const std::string &var_name);
  std::unordered_map<unsigned long, std::string> reverse() const;
};
}

#endif /* _VAR_INDEX_MANAGER_HPP_ */
