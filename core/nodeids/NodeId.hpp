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

namespace k2cache {
class NodeId {
public:
  static constexpr size_t DATA_SIZE = 8;

  explicit NodeId(const std::string &input_string);
  NodeId();
  explicit NodeId(long value);

  long get_value() const;

  bool operator==(const NodeId &other) const;

  bool is_any() const;

  friend std::ostream &operator<<(std::ostream &os, const NodeId &data);
  friend std::istream &operator>>(std::istream &is, NodeId &data);

  static constexpr long NODE_ANY = -9;

  static NodeId create_any();

private:
  static long string_to_long(const std::string &input_string);
  long data;
};
} // namespace k2cache

#endif // RDFCACHEK2_NODEID_HPP
