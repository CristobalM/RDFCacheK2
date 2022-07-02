//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_NODEIDSMANAGERFACTORY_HPP
#define RDFCACHEK2_NODEIDSMANAGERFACTORY_HPP
#include "CacheArgs.hpp"
#include "I_FileRWHandler.hpp"
#include "NodeIdsManager.hpp"
#include <memory>

namespace k2cache {
struct NodeIdsManagerFactory {
  static std::unique_ptr<NodeIdsManager> create(const CacheArgs &args);
  static std::unique_ptr<NodeIdsManager>
  create(std::unique_ptr<I_FileRWHandler> &&plain_ni_fh,
         std::unique_ptr<I_FileRWHandler> &&mapped_ni_fh,
         std::unique_ptr<I_FileRWHandler> &&logs_fh,
         std::unique_ptr<I_FileRWHandler> &&logs_counter_fh);
};
} // namespace k2cache

#endif // RDFCACHEK2_NODEIDSMANAGERFACTORY_HPP
