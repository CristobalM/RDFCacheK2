//
// Created by cristobal on 9/9/21.
//

#include <sstream>

#include "serialization_util.hpp"

#include "NodeId.hpp"
#include "exceptions.hpp"
namespace k2cache {
NodeId::NodeId(const std::string &input_string)
    : data(string_to_long(input_string)) {}

long NodeId::get_value() const { return data; }

bool NodeId::operator==(const NodeId &other) const {
  return data == other.data;
}
NodeId::NodeId(long value) : data(value) {}

long NodeId::string_to_long(const std::string &input_string) {
  if (input_string.size() != DATA_SIZE) {
    throw InvalidStringSizeForNodeId(input_string.size());
  }
  std::istringstream iss(input_string);
  return (long)read_u64(iss);
}

std::ostream &operator<<(std::ostream &os, const NodeId &data) {
  write_u64(os, (unsigned long)data.data);
  return os;
}

std::istream &operator>>(std::istream &is, NodeId &data) {
  auto value = (long)read_u64(is);
  data = NodeId(value);
  return is;
}
NodeId::NodeId() : data(0) {}
bool NodeId::is_any() const { return data == NODE_ANY; }
NodeId NodeId::create_any() { return NodeId(NODE_ANY); }
} // namespace k2cache