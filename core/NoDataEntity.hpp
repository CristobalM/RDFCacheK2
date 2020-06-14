//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_NODATAENTITY_HPP
#define RDFCACHEK2_NODATAENTITY_HPP

#include <exception>

class NoDataEntity {
  static std::unique_ptr<NoDataEntity>
  create_from_bytes_string(const std::string &bytes_string) {
    throw std::runtime_error(
        "attempt to create_from_bytes_string in NoDataEntity");
  }

  std::string to_bytes_string() {
    throw std::runtime_error(
        "attempt serialize (to_bytes_string) in NoDataEntity");
  }
};

#endif // RDFCACHEK2_NODATAENTITY_HPP
