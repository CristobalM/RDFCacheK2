//
// Created by cristobal on 20-11-21.
//

#ifndef RDFCACHEK2_NODESSEQUENCE_HPP
#define RDFCACHEK2_NODESSEQUENCE_HPP

#include <string>
#include <vector>
#include "I_OStream.hpp"

namespace k2cache {
struct I_IStream;
class NodesSequence {
  std::vector<uint64_t> values;

public:
  explicit NodesSequence(std::vector<uint64_t> &&values);

  uint64_t get_id(uint64_t value);
  uint64_t get_real_id(uint64_t position, int *err_code);

  uint64_t get_last_assigned();

  static NodesSequence from_input_stream(I_IStream &input_stream);

  void serialize(I_OStream &output_stream);

  const std::vector<uint64_t> &get_values();
};
} // namespace k2cache

#endif // RDFCACHEK2_NODESSEQUENCE_HPP
