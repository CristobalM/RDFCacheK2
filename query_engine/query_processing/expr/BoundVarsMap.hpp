//
// Created by cristobal on 5/18/21.
//

#ifndef RDFCACHEK2_BOUNDVARSMAP_HPP
#define RDFCACHEK2_BOUNDVARSMAP_HPP

#include <RDFTriple.hpp>
#include <string>
class BoundVarsMap {
  std::unordered_map<std::string, RDFResource> bound_map;

public:
  void bind(const std::string &var_name, RDFResource &&resource);
  bool has_var(const std::string &var_name) const;
  RDFResource get_resource_from(const std::string &var_name) const;
};

#endif // RDFCACHEK2_BOUNDVARSMAP_HPP
