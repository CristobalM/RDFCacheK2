//
// Created by Cristobal Miranda, 2020
//

#include <netinet/in.h>

#include "serialization_util.hpp"

void write_u32(std::ostream &output_stream, uint32_t value) {
  value = htonl(value);
  output_stream.write(reinterpret_cast<char *>(&value), sizeof(uint32_t));
}

uint32_t read_u32(std::istream &input_stream) {
  uint32_t result;
  input_stream.read(reinterpret_cast<char *>(&result), sizeof(uint32_t));
  result = ntohl(result);
  return result;
}