//
// Created by cristobal on 9/9/21.
//

#include <sstream>

#include "InvalidStringSizeForNodeId.hpp"
#include "NodeId.hpp"

#include <serialization_util.hpp>
//
// NodeId NodeId::create_from_string(const std::string &input_string) {
//  return NodeId(input_string);
//}
NodeId::NodeId(const std::string &input_string)
    : data(string_to_ul(input_string)) {}
// std::string NodeId::get_string() const { return {data.begin(), data.end()}; }
unsigned long NodeId::string_to_ul(const std::string &input_string) {
  if (input_string.size() != DATA_SIZE) {
    throw InvalidStringSizeForNodeId(input_string.size());
  }
  std::istringstream iss(input_string);
  return read_u64(iss);
}
unsigned long NodeId::get_raw() const { return data; }

// uint8_t NodeId::get_type_byte() const { return data[0]; }
// int NodeId::get_value1() const {
//  int result;
//  std::copy(data.begin() + 1, data.begin() + 5, &result);
//  return result;
//}
// long NodeId::get_value2() const {
//  long result;
//  std::copy(data.begin() + 5, data.end(), &result);
//  return result;
//}
//
// NodeId::NodeId(uint8_t type_byte, int value1, long value2) {
//  data[0] = type_byte;
//  std::copy(reinterpret_cast<char *>(&value1),
//            reinterpret_cast<char *>(&value1) + sizeof(value1),
//            data.begin() + 1);
//  std::copy(reinterpret_cast<char *>(&value2),
//            reinterpret_cast<char *>(&value2) + sizeof(value2),
//            data.begin() + 5);
//}

std::ostream &operator<<(std::ostream &os, const NodeId &data) {
  //  uint8_t byte = data.get_type_byte();
  //  int value1 = data.get_value1();
  //  long value2 = data.get_value2();
  //  os.write(reinterpret_cast<char *>(&byte), 1);
  //  write_u32(os, value1);
  write_u64(os, data.data);
  return os;
}

std::istream &operator>>(std::istream &is, NodeId &data) {
  //  uint8_t type_byte;
  //  int value1;
  //  long value2;
  //  is.read(reinterpret_cast<char *>(&type_byte), 1);
  //  value1 = (int)read_u32(is);
  //  value2 = (long)read_u64(is);
  auto value = read_u64(is);
  data = NodeId(value);
  return is;
}
bool NodeId::operator==(const NodeId &other) const {
  return data == other.data;
}
NodeId::NodeId(uint64_t value) : data(value) {}
