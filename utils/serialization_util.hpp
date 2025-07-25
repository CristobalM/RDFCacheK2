//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_SERIALIZATION_UTIL_HPP
#define RDFCACHEK2_SERIALIZATION_UTIL_HPP

#include <cstdint>
#include <fstream>
#include <memory>
namespace k2cache {
void write_u16(std::ostream &output_stream, uint16_t value);
uint16_t read_u16(std::istream &input_stream);

void write_u32(std::ostream &output_stream, uint32_t value);
uint32_t read_u32(std::istream &input_stream);

void write_u64(std::ostream &output_stream, uint64_t value);
uint64_t read_u64(std::istream &input_stream);

void write_u32_array(std::ostream &output_stream, uint32_t *array,
                     uint32_t array_size);
std::unique_ptr<uint32_t[]> read_u32_array(std::istream &input_stream,
                                           uint32_t *array_size);
} // namespace k2cache
#endif // RDFCACHEK2_SERIALIZATION_UTIL_HPP
