//
// Created by cristobal on 7/12/21.
//

#ifndef RDFCACHEK2_BGPOPSFACTORY_HPP
#define RDFCACHEK2_BGPOPSFACTORY_HPP

#include "BGPOp.hpp"
#include <K2TreeMixed.hpp>
#include <TimeControl.hpp>
#include <memory>
#include <query_processing/Triple.hpp>
#include <query_processing/VarBindingQProc.hpp>
#include <query_processing/VarIndexManager.hpp>
class BGPOpsFactory {
public:
  static std::unique_ptr<BGPOp> create_bgp_op(
      std::unique_ptr<K2TreeScanner> &&scanner, Triple &triple,
      VarIndexManager &vim, std::vector<long> &join_incidence,
      std::unordered_map<unsigned long, unsigned long> &header_rev_map,
      TimeControl &time_control, VarBindingQProc &var_binding_qproc);
};

#endif // RDFCACHEK2_BGPOPSFACTORY_HPP
