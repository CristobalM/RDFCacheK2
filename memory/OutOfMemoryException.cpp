//
// Created by cristobal on 9/30/21.
//

#include <string>
#include "OutOfMemoryException.hpp"
namespace k2cache {
OutOfMemoryException::OutOfMemoryException(uint64_t required_bytes,
                                           uint64_t capacity_bytes)
    : std::runtime_error(
          "OutOfMemoryError: Required " + std::to_string(required_bytes) +
          " which surpasses capacity " + std::to_string(capacity_bytes)) {}
} // namespace k2cache