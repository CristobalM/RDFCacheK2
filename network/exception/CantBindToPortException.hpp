//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CANTBINDTOPORTEXCEPTION_HPP
#define RDFCACHEK2_CANTBINDTOPORTEXCEPTION_HPP

#include <stdexcept>
#include <string>

class CantBindToPortException : std::runtime_error {
public:
  CantBindToPortException(uint16_t port)
      : std::runtime_error("Cant bind to port " + std::to_string(port)) {}
};
#endif // RDFCACHEK2_CANTBINDTOPORTEXCEPTION_HPP
