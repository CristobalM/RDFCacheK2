//
// Created by cristobal on 9/9/21.
//

#ifndef RDFCACHEK2_INVALIDSTRINGSIZEFORNODEID_HPP
#define RDFCACHEK2_INVALIDSTRINGSIZEFORNODEID_HPP

#include <stdexcept>

class InvalidStringSizeForNodeId : public std::runtime_error {
public:
  explicit InvalidStringSizeForNodeId(size_t given_size)
      : std::runtime_error("Invalid string size for NodeId, size given: " +
                           std::to_string(given_size)) {}
};

#endif // RDFCACHEK2_INVALIDSTRINGSIZEFORNODEID_HPP
