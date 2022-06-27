//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_NODEIDSMANAGERIMPL_HPP
#define RDFCACHEK2_NODEIDSMANAGERIMPL_HPP

#include "I_FileRWHandler.hpp"
#include "NodeIdsManager.hpp"
#include "NodesMap.hpp"
#include "NodesSequence.hpp"
#include <memory>

namespace k2cache {
class NodeIdsManagerImpl : public NodeIdsManager {
  std::unique_ptr<I_FileRWHandler> plain_ni_fh;
  std::unique_ptr<I_FileRWHandler> mapped_ni_fh;

  std::unique_ptr<NodesSequence> nodes_sequence;
  std::unique_ptr<NodesMap> nodes_map;

public:
  NodeIdsManagerImpl(std::unique_ptr<I_FileRWHandler> &&plain_ni_fh,
                     std::unique_ptr<I_FileRWHandler> &&mapped_ni_fh);

  NodesSequence &get_nodes_sequence() override;
  long get_id(long real_id) override;
};
} // namespace k2cache

#endif // RDFCACHEK2_NODEIDSMANAGERIMPL_HPP
