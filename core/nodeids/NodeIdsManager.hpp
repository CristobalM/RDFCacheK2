//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_NODEIDSMANAGER_HPP
#define RDFCACHEK2_NODEIDSMANAGER_HPP

#include "CacheArgs.hpp"
#include "I_FileRWHandler.hpp"
#include "NodesSequence.hpp"
#include <memory>

namespace k2cache {
class NodeIdsManager {
  std::unique_ptr<I_FileRWHandler> plain_ni_fh;
  std::unique_ptr<I_FileRWHandler> mapped_ni_fh;

  std::unique_ptr<NodesSequence> plain_nodes_sequence;

public:
  NodeIdsManager(std::unique_ptr<I_FileRWHandler> &&plain_ni_fh,
                 std::unique_ptr<I_FileRWHandler> &&mapped_ni_fh);

  explicit NodeIdsManager(const CacheArgs &args);

  NodesSequence &get_nodes_sequence();
};
} // namespace k2cache

#endif // RDFCACHEK2_NODEIDSMANAGER_HPP
