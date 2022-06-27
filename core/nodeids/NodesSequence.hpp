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
  static constexpr long NOT_FOUND = -1;

  explicit NodesSequence(std::vector<long> &&values);

  long get_id(long value);

  long get_value(long position);

  static NodesSequence from_input_stream(I_IStream &input_stream);
};
} // namespace k2cache

#endif // RDFCACHEK2_NODESSEQUENCE_HPP
