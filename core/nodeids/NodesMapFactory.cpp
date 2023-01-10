//
// Created by cristobal on 26-06-22.
//
#include "NodesMapFactory.hpp"
#include "NodesMapImpl.hpp"
#include "node_ids_constants.hpp"
#include <map>
#include <serialization_util.hpp>
namespace k2cache {
std::unique_ptr<NodesMap>
NodesMapFactory::from_input_stream(I_IStream &input_stream) {
  std::map<uint64_t, uint64_t> resulting;
  auto &is = input_stream.get_istream();
  auto impl_id = (int)read_u32(is);

  // we only have one implementation at the moment
  if (impl_id != STD_MAP_NI_IMPL)
    throw std::runtime_error("invalid node ids map implementation id: " +
                             std::to_string(impl_id));

  auto map_size = read_u64(is);
  for (auto i = 0UL; i < map_size; i++) {
    auto key = read_u64(is);
    auto value = read_u64(is);
    resulting[key] = value;
  }
  return std::make_unique<NodesMapImpl>(std::move(resulting));
}

} // namespace k2cache
