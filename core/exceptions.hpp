//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_EXCEPT_HPP
#define RDFCACHEK2_EXCEPT_HPP

#include <stdexcept>
#include <string>

namespace k2cache {
class PredicateNotFound : std::runtime_error {
public:
  explicit PredicateNotFound(const ulong predicate_index)
      : std::runtime_error("Not found predicate with index " +
                           std::to_string(predicate_index)) {}
};

class PredicateAlreadyExists : std::runtime_error {
public:
  explicit PredicateAlreadyExists(const ulong predicate_index)
      : std::runtime_error("There is already one predicate with the index " +
                           std::to_string(predicate_index)) {}
};

class InvalidStringSizeForNodeId : public std::runtime_error {
public:
  explicit InvalidStringSizeForNodeId(size_t given_size)
      : std::runtime_error("Invalid string size for NodeId, size given: " +
                           std::to_string(given_size)) {}
};
} // namespace k2cache

#endif // RDFCACHEK2_EXCEPT_HPP
