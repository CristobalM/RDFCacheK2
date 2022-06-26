//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_PREDICATESCACHEANDIDSMANAGER_HPP
#define RDFCACHEK2_PREDICATESCACHEANDIDSMANAGER_HPP
#include "PredicatesCacheManager.hpp"
#include "nodeids/NodeIdsManager.hpp"
#include <memory>
namespace k2cache {
class PredicatesCacheAndIdsManager {
  std::unique_ptr<PredicatesCacheManager> &&pcm;
  std::unique_ptr<NodeIdsManager> &&nis;

public:
  PredicatesCacheAndIdsManager(std::unique_ptr<PredicatesCacheManager> &&pcm,
                               std::unique_ptr<NodeIdsManager> &&nis);
};
} // namespace k2cache

#endif // RDFCACHEK2_PREDICATESCACHEANDIDSMANAGER_HPP
