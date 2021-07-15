//
// Created by cristobal on 7/12/21.
//

#include "BGPOpsFactory.hpp"
#include "OneVarCPBGPOp.hpp"
#include "OneVarIntersectBGPOp.hpp"
#include "TripleExistenceBGPOP.hpp"
#include "TwoVarCProductBGPOp.hpp"
#include "TwoVarIntersectionBGPOp.hpp"
#include "TwoVarJoinOneBGPOp.hpp"
std::unique_ptr<BGPOp> BGPOpsFactory::create_bgp_op(
    std::unique_ptr<K2TreeMixed::K2TreeScanner> &&scanner, Triple triple,
    VarIndexManager &vim, std::vector<long> &join_incidence,
    std::unordered_map<unsigned long, unsigned long> &header_rev_map) {
  if (triple.subject.type == TermType::VAR &&
      triple.object.type == TermType::VAR) {
    unsigned long subject_pos =
        header_rev_map[vim.var_indexes[triple.subject.value]];
    unsigned long object_pos =
        header_rev_map[vim.var_indexes[triple.object.value]];
    auto subject_incidence = join_incidence[subject_pos];
    auto object_incidence = join_incidence[object_pos];
    join_incidence[subject_pos]++;
    join_incidence[object_pos]++;
    if (subject_incidence > 0 && object_incidence > 0) {
      return std::make_unique<TwoVarIntersectionBGPOp>(std::move(scanner),
                                                       subject_pos, object_pos);
    }
    if (subject_incidence > 0) {
      return std::make_unique<TwoVarJoinOneBGPOp<BGPOp::SUBJECT_VAR>>(
          std::move(scanner), subject_pos, object_pos);
    }
    if (object_incidence > 0) {
      return std::make_unique<TwoVarJoinOneBGPOp<BGPOp::OBJECT_VAR>>(
          std::move(scanner), subject_pos, object_pos);
    }
    // here none of the variables would be considered yet
    return std::make_unique<TwoVarCProductBGPOp>(std::move(scanner),
                                                 subject_pos, object_pos);
  }

  if (triple.subject.type == TermType::VAR) {
    unsigned long subject_pos =
        header_rev_map[vim.var_indexes[triple.subject.value]];
    auto subject_incidence = join_incidence[subject_pos];
    join_incidence[subject_pos]++;
    if (subject_incidence > 0) {
      return std::make_unique<OneVarIntersectBGPOp<BGPOp::SUBJECT_VAR>>(
          std::move(scanner), subject_pos);
    }
    return std::make_unique<OneVarCPBGPOp<BGPOp::SUBJECT_VAR>>(
        std::move(scanner), subject_pos);
  }

  if (triple.object.type == TermType::VAR) {
    unsigned long object_pos =
        header_rev_map[vim.var_indexes[triple.object.value]];
    auto object_incidence = join_incidence[object_pos];
    join_incidence[object_pos]++;
    if (object_incidence > 0) {
      return std::make_unique<OneVarIntersectBGPOp<BGPOp::OBJECT_VAR>>(
          std::move(scanner), object_pos);
    }
    return std::make_unique<OneVarCPBGPOp<BGPOp::OBJECT_VAR>>(
        std::move(scanner), object_pos);
  }

  // reaching this point means that there are no vars in the triple

  return std::make_unique<TripleExistenceBGPOP>(std::move(scanner), triple);
}
