//
// Created by Cristobal Miranda, 2020
//

#include <netinet/in.h>

#include <endian.h>

#include "serialization_util.hpp"

void write_u16(std::ostream &output_stream, uint16_t value) {
  value = htons(value);
  output_stream.write(reinterpret_cast<char *>(&value), sizeof(uint16_t));
}

uint16_t read_u16(std::istream &input_stream) {
  uint16_t result;
  input_stream.read(reinterpret_cast<char *>(&result), sizeof(uint16_t));
  result = ntohs(result);
  return result;
}

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

void write_u64(std::ostream &output_stream, uint64_t value) {
  value = htobe64(value);
  output_stream.write(reinterpret_cast<char *>(&value), sizeof(uint64_t));
}

uint64_t read_u64(std::istream &input_stream) {
  uint64_t result;
  input_stream.read(reinterpret_cast<char *>(&result), sizeof(uint64_t));
  result = be64toh(result);
  return result;
}
