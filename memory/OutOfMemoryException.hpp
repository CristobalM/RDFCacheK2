//
// Created by cristobal on 9/30/21.
//

#ifndef RDFCACHEK2_OUTOFMEMORYEXCEPTION_HPP
#define RDFCACHEK2_OUTOFMEMORYEXCEPTION_HPP

#include <stdexcept>
namespace k2cache {

class OutOfMemoryException : public std::runtime_error {
public:
  OutOfMemoryException(unsigned long required_bytes,
                       unsigned long capacity_bytes);
};
} // namespace k2cache
#endif // RDFCACHEK2_OUTOFMEMORYEXCEPTION_HPP
