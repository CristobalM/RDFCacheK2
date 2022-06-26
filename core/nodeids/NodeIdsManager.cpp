//
// Created by cristobal on 26-06-22.
//

#include "NodeIdsManager.hpp"
#include "NodesSequence.hpp"
namespace k2cache {
NodeIdsManager::NodeIdsManager(std::unique_ptr<I_FileRWHandler> &&plain_ni_fh,
                               std::unique_ptr<I_FileRWHandler> &&mapped_ni_fh)
    : plain_ni_fh(std::move(plain_ni_fh)),
      mapped_ni_fh(std::move(mapped_ni_fh)),
      plain_nodes_sequence(
          std::make_unique<NodesSequence>(NodesSequence::from_input_stream(
              *this->plain_ni_fh->get_reader(std::ios::binary)))) {}

} // namespace k2cache