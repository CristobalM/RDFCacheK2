//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_EXCEPT_HPP
#define RDFCACHEK2_EXCEPT_HPP

#include <stdexcept>
#include <string>

class PredicateNotFound : std::runtime_error {
public:
  explicit PredicateNotFound(const unsigned long predicate_index)
      : std::runtime_error("Not found predicate with index " +
                           std::to_string(predicate_index)) {}
};

class PredicateAlreadyExists : std::runtime_error {
public:
  explicit PredicateAlreadyExists(const unsigned long predicate_index)
      : std::runtime_error("There is already one predicate with the index " +
                           std::to_string(predicate_index)) {}
};

#endif // RDFCACHEK2_EXCEPT_HPP
