//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_ISDMANAGER_HPP
#define RDFCACHEK2_ISDMANAGER_HPP

#include "NodeId.hpp"
#include "RDFTriple.hpp"
#include <string>

class ISDManager {
public:
  virtual void save(const std::string &filename) = 0;
  virtual uint64_t get_resource_id_from_node_id(const NodeId &node_id) = 0;
  virtual NodeId get_node_id(uint64_t index) = 0;
  virtual unsigned long last_id() = 0;
  virtual ~ISDManager() = default;
};
#endif // RDFCACHEK2_ISDMANAGER_HPP
