//
// Created by cristobal on 20-11-21.
//

#include "NodesSequence.hpp"
#include "FileIStream.hpp"
#include "node_ids_constants.hpp"
#include "serialization_util.hpp"
#include <stdexcept>
namespace k2cache {
uint64_t NodesSequence::get_id(uint64_t value) {
  auto it = std::lower_bound(values.begin(), values.end(), value);
  if (it == values.end())
    return NOT_FOUND_NODEID;
  if (*it != value)
    return NOT_FOUND_NODEID;
  return it - values.begin();
}

uint64_t NodesSequence::get_real_id(uint64_t position, int *err_code) {
  if (position >= values.size()){
    if(err_code != nullptr){
      *err_code = (int)NidsErrCode::NOT_FOUND_ERR_CODE;
    }
    return -1;
  }
  return values[position];
}

NodesSequence::NodesSequence(std::vector<uint64_t> &&values)
    : values(std::move(values)) {}
NodesSequence NodesSequence::from_input_stream(I_IStream &input_stream) {
  std::vector<uint64_t> data;
  auto &is = input_stream.get_istream();
  auto nodes_number = (uint64_t)read_u64(is);
  data.reserve(nodes_number);
  for (uint64_t i = 0; i < nodes_number; i++) {
    auto node_id = read_u64(is);
    data.push_back(node_id);
  }
  return NodesSequence(std::move(data));
}
uint64_t NodesSequence::get_last_assigned() { return values.size() - 1; }

void NodesSequence::serialize(I_OStream &output_stream) {
  auto &os = output_stream.get_ostream();
  write_u64(os, values.size());
  for(auto v: values){
    write_u64(os, v);
  }
}

} // namespace k2cache