//
// Created by cristobal on 26-06-22.
//

#include "NodeIdsManager.hpp"
#include "FileRWHandler.hpp"
#include "NodesSequence.hpp"
namespace k2cache {
NodeIdsManager::NodeIdsManager(std::unique_ptr<I_FileRWHandler> &&plain_ni_fh,
                               std::unique_ptr<I_FileRWHandler> &&mapped_ni_fh)
    : plain_ni_fh(std::move(plain_ni_fh)),
      mapped_ni_fh(std::move(mapped_ni_fh)),
      plain_nodes_sequence(
          std::make_unique<NodesSequence>(NodesSequence::from_input_stream(
              *this->plain_ni_fh->get_reader(std::ios::binary)))) {}

NodeIdsManager::NodeIdsManager(const CacheArgs &args)
    : NodeIdsManager(
          std::make_unique<FileRWHandler>(args.node_ids_filename),
          std::make_unique<FileRWHandler>(args.mapped_node_ids_filename)) {}

NodesSequence &NodeIdsManager::get_nodes_sequence() {
  return *plain_nodes_sequence;
}

} // namespace k2cache
