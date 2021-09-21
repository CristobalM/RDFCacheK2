//
// Created by cristobal on 9/9/21.
//

#ifndef RDFCACHEK2_NODEID_HPP
#define RDFCACHEK2_NODEID_HPP

#include <array>
#include <cstdint>
#include <string>

class NodeId {
public:
  static constexpr size_t DATA_SIZE = 8;
  // using RAW = std::array<uint8_t, DATA_SIZE>;

  explicit NodeId(const std::string &input_string);
  NodeId() = default;
  explicit NodeId(uint64_t value);
  //  NodeId(uint8_t type_byte, int value1, long value2);

  // static NodeId create_from_string(const std::string &input_string);
  // std::string get_string() const;
  // const RAW &get_raw() const;
  unsigned long get_raw() const;

  // uint8_t get_type_byte() const;
  // int get_value1() const;
  // long get_value2() const;

  bool operator==(const NodeId &other) const;

  friend std::ostream &operator<<(std::ostream &os, const NodeId &data);
  friend std::istream &operator>>(std::istream &is, NodeId &data);

private:
  // static RAW string_to_array(const std::string &input_string);
  static unsigned long string_to_ul(const std::string &input_string);
  unsigned long data;
};

#endif // RDFCACHEK2_NODEID_HPP
