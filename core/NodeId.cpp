//
// Created by cristobal on 9/9/21.
//

#include <sstream>

#include <serialization_util.hpp>

#include "InvalidStringSizeForNodeId.hpp"
#include "NodeId.hpp"

NodeId::NodeId(const std::string &input_string)
    : data(string_to_ul(input_string)) {}

unsigned long NodeId::get_raw() const { return data; }

bool NodeId::operator==(const NodeId &other) const {
  return data == other.data;
}
NodeId::NodeId(unsigned long value) : data(value) {}

unsigned long NodeId::string_to_ul(const std::string &input_string) {
  if (input_string.size() != DATA_SIZE) {
    throw InvalidStringSizeForNodeId(input_string.size());
  }
  std::istringstream iss(input_string);
  return read_u64(iss);
}

std::ostream &operator<<(std::ostream &os, const NodeId &data) {
  write_u64(os, data.data);
  return os;
}

std::istream &operator>>(std::istream &is, NodeId &data) {
  auto value = read_u64(is);
  data = NodeId(value);
  return is;
}
NodeId::NodeId() : data(0) {}
