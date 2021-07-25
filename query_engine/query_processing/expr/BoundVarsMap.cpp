//
// Created by cristobal on 5/18/21.
//

#include "BoundVarsMap.hpp"
void BoundVarsMap::bind(const std::string &var_name, RDFResource &&resource) {
  bound_map[var_name] = std::move(resource);
}
RDFResource BoundVarsMap::get_resource_from(const std::string &var_name) const {
  return bound_map.at(var_name);
}
bool BoundVarsMap::has_var(const std::string &var_name) const {
  return bound_map.find(var_name) != bound_map.end();
}
