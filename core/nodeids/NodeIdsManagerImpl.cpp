//
// Created by cristobal on 26-06-22.
//

#include "NodeIdsManagerImpl.hpp"
#include "FileRWHandler.hpp"
#include "I_IStream.hpp"
#include "NodesMapFactory.hpp"
#include "NodesSequence.hpp"

namespace k2cache {
NodeIdsManagerImpl::NodeIdsManagerImpl(
    std::unique_ptr<I_FileRWHandler> &&plain_ni_fh,
    std::unique_ptr<I_FileRWHandler> &&mapped_ni_fh)
    : plain_ni_fh(std::move(plain_ni_fh)),
      mapped_ni_fh(std::move(mapped_ni_fh)),
      nodes_sequence(
          std::make_unique<NodesSequence>(NodesSequence::from_input_stream(
              *this->plain_ni_fh->get_reader(std::ios::binary)))),
      nodes_map(NodesMapFactory::from_input_stream(
          *this->mapped_ni_fh->get_reader(std::ios::binary))) {}

NodesSequence &NodeIdsManagerImpl::get_nodes_sequence() {
  return *nodes_sequence;
}
long NodeIdsManagerImpl::get_id(long real_id) {
  auto plain_id = nodes_sequence->get_id(real_id);
  if (plain_id != NodesSequence::NOT_FOUND)
    return plain_id;
  return nodes_map->get_id(real_id);
}

} // namespace k2cache
