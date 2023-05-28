//
// Created by cristobal on 7/12/21.
//

#ifndef RDFCACHEK2_BGPOPSFACTORY_HPP
#define RDFCACHEK2_BGPOPSFACTORY_HPP

#include "BGPOp.hpp"
#include "BgpMessage.hpp"
#include <TimeControl.hpp>
#include <memory>
#include <query_processing/VarIndexManager.hpp>
namespace k2cache {

class BGPOpsFactory {
public:
  static std::unique_ptr<BGPOp> create_bgp_op(
      std::unique_ptr<K2TreeScanner> &&scanner, BgpTriple &triple,
      VarIndexManager &vim, std::vector<long> &join_incidence,
      std::unordered_map<unsigned long, unsigned long> &header_rev_map,
      TimeControl &time_control);
};
}

#endif // RDFCACHEK2_BGPOPSFACTORY_HPP
