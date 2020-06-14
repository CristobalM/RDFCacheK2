//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_SERIALIZATION_UTIL_HPP
#define RDFCACHEK2_SERIALIZATION_UTIL_HPP

#include <cstdint>
#include <fstream>

void write_u32(std::ostream &output_stream, uint32_t value);
uint32_t read_u32(std::istream &input_stream);

#endif // RDFCACHEK2_SERIALIZATION_UTIL_HPP
