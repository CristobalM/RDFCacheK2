#ifndef EMPTY_ISD_MANAGER_HPP
#define EMPTY_ISD_MANAGER_HPP

#include "ISDManager.hpp"

class EmptyISDManager : public ISDManager {
public:
  unsigned long last_id() override;
  void save(const std::string &filename) override;
  uint64_t get_resource_id_from_node_id(const NodeId &node_id) override;
  NodeId get_node_id(uint64_t index) override;
  ~EmptyISDManager() override = default;
};

#endif