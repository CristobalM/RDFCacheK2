//
// Created by cristobal on 9/9/21.
//

#ifndef RDFCACHEK2_NODEID_HPP
#define RDFCACHEK2_NODEID_HPP

#include <array>
#include <cstdint>
#include <string>

#include <istream>
#include <ostream>

#include "serialization_util.hpp"

class NodeId {
public:
  static constexpr size_t DATA_SIZE = 8;

  explicit NodeId(const std::string &input_string);
  NodeId();
  explicit NodeId(unsigned long value);

  unsigned long get_raw() const;

  bool operator==(const NodeId &other) const;

  friend std::ostream &operator<<(std::ostream &os, const NodeId &data);
  friend std::istream &operator>>(std::istream &is, NodeId &data);

private:
  static unsigned long string_to_ul(const std::string &input_string);
  unsigned long data;
};

#endif // RDFCACHEK2_NODEID_HPP
