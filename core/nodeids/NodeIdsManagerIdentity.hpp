//
// Created by cristobal on 30-06-22.
//

#ifndef RDFCACHEK2_NODEIDSMANAGERIDENTITY_HPP
#define RDFCACHEK2_NODEIDSMANAGERIDENTITY_HPP

#include "NodeIdsManager.hpp"
namespace k2cache {
class NodeIdsManagerIdentity : public NodeIdsManager {
public:
  long get_id(long real_id) override;
  long get_real_id(long mapped_id) override;
  long get_id_or_create(long real_id) override;
};
} // namespace k2cache

#endif // RDFCACHEK2_NODEIDSMANAGERIDENTITY_HPP
