#include "EmptyISDManager.hpp"
#include <stdexcept>

unsigned long EmptyISDManager::last_id() { return 0; }
void EmptyISDManager::save(const std::string &) {
  throw std::runtime_error("save Not implemented");
}
uint64_t EmptyISDManager::get_resource_id_from_node_id(const NodeId &) {
  throw std::runtime_error("get_resource_id_from_node_id Not implemented");
}
NodeId EmptyISDManager::get_node_id(uint64_t) {
  throw std::runtime_error("get_node_id Not implemented");
}
