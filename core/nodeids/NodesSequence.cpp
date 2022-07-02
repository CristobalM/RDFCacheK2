//
// Created by cristobal on 20-11-21.
//

#include "NodesSequence.hpp"
#include "FileIStream.hpp"
#include "node_ids_constants.hpp"
#include "serialization_util.hpp"
#include <stdexcept>
namespace k2cache {
long NodesSequence::get_id(long value) {
  auto it = std::lower_bound(values.begin(), values.end(), value);
  if (it == values.end())
    return NOT_FOUND_NODEID;
  if (*it != value)
    return NOT_FOUND_NODEID;
  return it - values.begin();
}

long NodesSequence::get_real_id(long position) {
  if (position >= (long)values.size())
    return NOT_FOUND_NODEID;
  return values[position];
}

NodesSequence::NodesSequence(std::vector<long> &&values)
    : values(std::move(values)) {}
NodesSequence NodesSequence::from_input_stream(I_IStream &input_stream) {
  std::vector<long> data;
  auto &is = input_stream.get_istream();
  auto nodes_number = (unsigned long)read_u64(is);
  data.reserve(nodes_number);
  for (unsigned long i = 0; i < nodes_number; i++) {
    auto node_id = (long)read_u64(is);
    data.push_back(node_id);
  }
  return NodesSequence(std::move(data));
}
long NodesSequence::get_last_assigned() { return (long)values.size() - 1; }

} // namespace k2cache