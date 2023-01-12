//
// Created by cristobal on 30-06-22.
//

#ifndef RDFCACHEK2_NODEIDSMANAGERIDENTITY_HPP
#define RDFCACHEK2_NODEIDSMANAGERIDENTITY_HPP

#include "NodeIdsManager.hpp"
namespace k2cache {
class NodeIdsManagerIdentity : public NodeIdsManager {
public:
  uint64_t get_id(uint64_t real_id) override;
  uint64_t get_real_id(uint64_t mapped_id, int *err_code) override;
  uint64_t get_id_or_create(uint64_t real_id) override;
};
} // namespace k2cache

#endif // RDFCACHEK2_NODEIDSMANAGERIDENTITY_HPP
