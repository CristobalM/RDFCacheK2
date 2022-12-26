//
// Created by cristobal on 20-11-21.
//

#ifndef RDFCACHEK2_NODESSEQUENCE_HPP
#define RDFCACHEK2_NODESSEQUENCE_HPP

#include <string>
#include <vector>

namespace k2cache {
struct I_IStream;
class NodesSequence {
  std::vector<long> values;

public:
  explicit NodesSequence(std::vector<long> &&values);

  long get_id(long value);
  long get_real_id(long position, int *err_code);

  long get_last_assigned();

  static NodesSequence from_input_stream(I_IStream &input_stream);
};
} // namespace k2cache

#endif // RDFCACHEK2_NODESSEQUENCE_HPP
