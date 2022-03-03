//
// Created by cristobal on 9/30/21.
//

#include <string>
#include "OutOfMemoryException.hpp"
OutOfMemoryException::OutOfMemoryException(unsigned long required_bytes,
                                           unsigned long capacity_bytes)
    : std::runtime_error(
          "OutOfMemoryError: Required " + std::to_string(required_bytes) +
          " which surpasses capacity " + std::to_string(capacity_bytes)) {}
