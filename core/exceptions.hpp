//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_EXCEPT_HPP
#define RDFCACHEK2_EXCEPT_HPP

#include <stdexcept>
#include <string>

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

class RootNotFound : std::runtime_error {
public:
  RootNotFound()
      : std::runtime_error(
            "Root block was not found when reading binary string") {}
};

#endif // RDFCACHEK2_EXCEPT_HPP
